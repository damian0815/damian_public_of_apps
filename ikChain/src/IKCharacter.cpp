/*
 *  IKCharacter.cpp
 *  ikChain
 *
 *  Created by damian on 25/06/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "IKCharacter.h"
#include <queue>
using namespace std;

static const int DEBUG_BONE = -1;

//#define SWAP_WORLD_POSITION_HANDEDNESS
#define SWAP_MATRIX_HANDEDNESS

//#define MATRIX_SWAP_YZ
//#define MATRIX_SWAP_XZ
#define MATRIX_SWAP_XY

void IKCharacter::setup( CalSkeleton* cal_skel )
{
	skeleton = cal_skel;
	
	
	// find leaf bones
	deque<int> current;
	vector<int> roots = cal_skel->getCoreSkeleton()->getVectorRootCoreBoneId();
	current.insert( current.begin(), roots.begin(), roots.end() );
	while( !current.empty() )
	{
		CalCoreBone* check = skeleton->getCoreSkeleton()->getCoreBone( current.front() );
		list<int> children = check->getListChildId();
		if ( children.size()==0 )
		{
			// this is a leaf
			leaf_bones.push_back( current.front() );
			printf("found leaf: %s\n", skeleton->getCoreSkeleton()->getCoreBone( leaf_bones.back() )->getName().c_str() );
		}
		else
		{
			// not a leaf
			if ( current.front()== DEBUG_BONE )
			{
				printf(" -- %i is %s\n", DEBUG_BONE, skeleton->getCoreSkeleton()->getCoreBone( DEBUG_BONE )->getName().c_str() );
			}
			current.insert( current.end(), children.begin(), children.end() );
		}
		
		// store length
		if ( check->getParentId()!= -1 )
		{
			CalCoreBone* parent = skeleton->getCoreSkeleton()->getCoreBone( check->getParentId() );
			CalVector delta = check->getTranslationAbsolute() - parent->getTranslationAbsolute();
			bone_lengths[check->getParentId()] = delta.length();
		}
		else
			bone_lengths[current.front()] = 0.0001f;
		
		current.pop_front();
	}

	// release all leaves
	for ( int i=0; i<leaf_bones.size() ;i++ )
	{
		weight_centres[leaf_bones[i]] = 0;
	}
	// pin all roots
	for ( int i=0; i<roots.size(); i++ )
	{
		weight_centres[roots[i]] = 1;
	}
	
	// pin everything whose parent has >1 children
	vector<CalCoreBone*> all_bones = skeleton->getCoreSkeleton()->getVectorCoreBone();
	for( int i=0; i<all_bones.size() ;i++ )
	{
		CalCoreBone* bone = all_bones[i];
		if ( bone->getParentId() == -1 )
			continue;
		CalCoreBone* parent = skeleton->getCoreSkeleton()->getCoreBone( bone->getParentId() );
		if ( parent->getListChildId().size() >1 )
			weight_centres[bone->getId()] = 1;
	}
	
	// fetch world positions from Cal3D model
	pullWorldPositions();
	
	// set targets
	for ( int i=0; i<leaf_bones.size(); i++ )
	{
		CalVector p = world_positions[leaf_bones[i]];
		setTarget( leaf_bones[i], ofxVec3f(p.x,p.y,p.z) );
	}
	
	// can we just push it straight back?
	pushWorldPositions();
	
}


void IKCharacter::setTarget( int which_leaf, ofxVec3f pos )
{
	CalVector p(pos.x, pos.y, pos.z );
	leaf_targets[which_leaf] = p;
}

void IKCharacter::pullWorldPositions()
{
	// get all the bones
	vector<CalBone*>& bones = skeleton->getVectorBone();
	// for each
	for ( int i=0; i<bones.size(); i++) 
	{
		int id= bones[i]->getCoreBone()->getId();
		world_positions[id] = bones[i]->getTranslationAbsolute();
	}
	
	swapWorldPositionHandedness();
	
}





ofxQuaternion IKCharacter::getRotationForParentBone( int bone_id, CalVector new_bone_pos_world, CalVector new_parent_pos_world )
{
	CalCoreSkeleton* core_skel = skeleton->getCoreSkeleton();
	CalBone* bone = skeleton->getBone( bone_id );
	int parent_id = core_skel->getCoreBone( bone_id )->getParentId();
	CalBone* parent_bone = skeleton->getBone( parent_id );

	// get the bone and parent bone's core absolute (==world) translations
	CalVector old_bone_pos_world = bone->getCoreBone()->getTranslationAbsolute();
	CalVector old_parent_pos_world = parent_bone->getCoreBone()->getTranslationAbsolute();
	
	// get a direction vector for each 
	CalVector old_dir = old_bone_pos_world - old_parent_pos_world;
	CalVector new_dir = new_bone_pos_world - new_parent_pos_world;
	old_dir.normalize();
	new_dir.normalize();
	// cancel out parents' parents' rotation
	int parent_parent_id = parent_bone->getCoreBone()->getParentId();
	if ( parent_parent_id != -1 )
	{
		CalBone* parent_parent_bone = skeleton->getBone( parent_parent_id );
		CalQuaternion old_parent_rot = parent_parent_bone->getCoreBone()->getRotationAbsolute();
		CalQuaternion new_parent_rot = parent_parent_bone->getRotationAbsolute();
		old_parent_rot.invert();
		new_parent_rot.invert();
		old_dir *= old_parent_rot;
		new_dir *= new_parent_rot;
	}
	
	// rotate from one to the other
	ofxQuaternion rot;
	if ( DEBUG_BONE == -1 || (bone_id == DEBUG_BONE) )
	{
		ofxVec3f od( old_dir.x, old_dir.y, old_dir.z );
		ofxVec3f nd( new_dir.x, new_dir.y, new_dir.z );
		rot.makeRotate( od, nd );
		// the result needs to be reflected to make it work with cal3d
		//rot.set( rot.x(), rot.y(), rot.z(), rot.w() );
	}
	
	// return
	return rot;
}


void swapHandedness( ofxMatrix4x4 & m )
{

/*
 if your matrix looks like this:
 { rx, ry, rz, 0 }
 { ux, uy, uz, 0 }
 { lx, ly, lz, 0 }
 { px, py, pz, 1 }
 
 To change it from left to right or right to left, flip it like this:
 { rx, rz, ry, 0 }
 { lx, lz, ly, 0 }
 { ux, uz, uy, 0 }
 { px, pz, py, 1 }
*/

#if defined MATRIX_SWAP_YZ
	static const int A = 1;
	static const int B = 2;
#elif defined MATRIX_SWAP_XZ
	static const int A = 0;
	static const int B = 2;
#elif defined MATRIX_SWAP_XY
	static const int A = 0;
	static const int B = 1;
#endif

	// first swap columns
	for ( int i=0; i<4; i++ )
	{
		float swap =   m._mat[i][A];
		m._mat[i][A] = m._mat[i][B];
		m._mat[i][B] = swap;
	}
	// then swap rows
	for ( int i=0; i<4; i++ )
	{
		float swap =   m._mat[A][i];
		m._mat[A][i] = m._mat[B][i];
		m._mat[B][i] = swap;
	}
	
	
}


void IKCharacter::pushWorldPositions()
{
	
	swapWorldPositionHandedness();
	
	// start at the roots
	vector<int> roots = skeleton->getCoreSkeleton()->getVectorRootCoreBoneId();
	deque<int> queue;
	for ( int i=0; i<roots.size(); i++ )
	{
		// add all children
		list<int>& children = skeleton->getCoreSkeleton()->getCoreBone( roots[i] )->getListChildId();
		for ( list<int>::iterator it =children.begin() ;it != children.end(); ++it )
		{
			queue.push_back( *it );
		}
	}
	
	while ( !queue.empty() )
	{
		// get bone and parent bone ids
		int bone_id = queue.front();
		CalBone* bone = skeleton->getBone( bone_id );
		queue.pop_front();
		int parent_id = bone->getCoreBone()->getParentId();
		// get new world positions
		CalVector b,p;
		b = world_positions[bone_id];
		p = world_positions[parent_id];
		// get the rotation
		ofxQuaternion rot = getRotationForParentBone( bone_id, b, p );
		

#ifdef SWAP_MATRIX_HANDEDNESS
		// FLIP THE ROTATION FROM LEFTHANDED to RIGHTHANDED gnashgnashgnash
		ofxMatrix4x4 flippy( rot );
		swapHandedness( flippy );
		rot.set( flippy );
#endif
		 
		
		// apply the rotation
		CalBone* parent_bone = skeleton->getBone( parent_id );
		CalBone* bone_to_set = parent_bone;
		bone_to_set->setRotation( bone_to_set->getCoreBone()->getRotation()*CalQuaternion(rot.x(), rot.y(), rot.z(), rot.w() ));
		bone_to_set->calculateState();
		
		// add all children
		list<int>& children = bone->getCoreBone()->getListChildId();
		for ( list<int>::iterator it =children.begin() ;it != children.end(); ++it )
		{
			queue.push_back( *it );
		}
		
	}
	
	skeleton->calculateState();

	swapWorldPositionHandedness();
	
}

void IKCharacter::swapWorldPositionHandedness()
{
#ifdef SWAP_WORLD_POSITION_HANDEDNESS
	// convert handedness back
	for ( map<int,CalVector>::iterator it = world_positions.begin(); it != world_positions.end(); ++it )
	{
		CalVector& p = (*it).second;
		float temp = p.y;
		p.y = p.z;
		p.z = temp;
	}
#endif
	
}	

void IKCharacter::resetToRest()
{
	vector<int> roots = skeleton->getCoreSkeleton()->getVectorRootCoreBoneId();
	for ( int i=0; i<roots.size(); i++ )
	{
		skeleton->getBone( roots[i] )->setCoreStateRecursive();
	}
	
}



void IKCharacter::solve( int iterations )
{
	pullWorldPositions();

	// start at leaf nodes
	deque<int> queue;
	
	// push leaf bones to target positions
	for ( int i=0; i<leaf_bones.size(); i++ )
	{
		// if we have a target for this one
		if ( leaf_targets.find(leaf_bones[i]) != leaf_targets.end() )
		{
			// set it
			world_positions[leaf_bones[i]] = leaf_targets[leaf_bones[i]];
		}
		int parent_id = skeleton->getCoreSkeleton()->getCoreBone( leaf_bones[i] )->getParentId();
		if ( parent_id != -1 )
			queue.push_back( parent_id );
	}

	while ( iterations>0 )
	{
		// queue to handle branching
		deque<int> branch_queue;
		while ( !queue.empty() || !branch_queue.empty() )
		{
			// if main queue is empty then we are ready to deal with branches
			if ( queue.empty() )
			{
				queue.push_back( branch_queue.front() );
				branch_queue.pop_front();
			}
			
			int next_id = queue.front();
			queue.pop_front();
			CalBone* bone = skeleton->getBone( next_id );
			list<int>& children = bone->getCoreBone()->getListChildId();
			// is this a branch?
			if ( children.size()>1 )
			{
				// still things to process -- push to branch queue
				if ( !queue.empty() )
				{
					branch_queue.push_back( next_id );
					continue;
				}
				// otherwise, process branch here
				
				
			}
			else
			{
				// children.size() is exactly 1
				assert( children.size()==1 );
				int child_id = children.front();
				
				// child_pos
				CalVector& b_c = world_positions[child_id];
				// current pos
				CalVector& b_p = world_positions[next_id];
				
				// now, the bone is the wrong length. correct its length to fulfil size constraint.
				CalVector delta = b_c - b_p;
				float length = delta.length();
				length = max( 0.00001f, length );
				// pointing from parent to child
				CalVector direction = delta/length;
				
				float desired_length = getBoneLength(next_id);
				float delta_length = desired_length - length;
				
				// balance according to weight_centre
				float weight_centre = getWeightCentre(next_id);
				
				// move
				b_c += weight_centre * delta_length * direction;
				b_p -= (1.0f-weight_centre) * delta_length * direction;
				
				
				// add parent
				queue.push_back( bone->getCoreBone()->getParentId() );
			}
		}	
		iterations--;
	}
	
	pushWorldPositions();
	
}





void IKCharacter::draw( int bone_id, float scale )
{
	CalBone* bone = skeleton->getBone( bone_id );
	int parent_id = bone->getCoreBone()->getParentId();
	if ( parent_id != -1 )
	{
		CalBone* parent = skeleton->getBone( parent_id );
		glBegin( GL_LINES );
		CalVector v = parent->getTranslationAbsolute();
		v*=scale;
		glVertex3f( v.x, v.y, v.z );
		v = bone->getTranslationAbsolute();
		v*=scale;
		glVertex3f( v.x, v.y, v.z );
		glEnd();

		glPushAttrib( GL_CURRENT_BIT );
		glColor3f( 0.1f, 0.8f, 0.1f );
		glBegin( GL_LINES );
		v = world_positions[parent_id];
		v*=scale;
		glVertex3f( v.x, v.y, v.z );
		v = world_positions[bone_id];
		v*=scale;
		glVertex3f( v.x, v.y, v.z );
		glEnd();
		glPopAttrib();
	}
	
	
	list<int> children = bone->getCoreBone()->getListChildId();
	for ( list<int>::iterator it = children.begin(); 
		 it != children.end();
		 ++it )
	{
		draw( *it, scale );
	}
	
	
}

void IKCharacter::draw( float scale )
{
	vector<int> roots = skeleton->getCoreSkeleton()->getVectorRootCoreBoneId();
	for ( int i=0; i<roots.size(); i++ )
	{
		draw( roots[i], scale  );
	}

	glPushMatrix();
	glScalef( scale, scale, scale );
	glPushAttrib( GL_CURRENT_BIT );
	glColor3f( 0.8f, 0.1f, 0.1f );
	glBegin( GL_TRIANGLES );
	for ( map<int,CalVector>::iterator it = leaf_targets.begin();
		 it != leaf_targets.end(); 
		 ++it )
	{
		CalVector target_pos = (*it).second;
		glVertex3f( target_pos.x-0.1, target_pos.y-0.05, target_pos.z );
		glVertex3f( target_pos.x+0.1, target_pos.y-0.05, target_pos.z );
		glVertex3f( target_pos.x, target_pos.y+0.05, target_pos.z );
	}
	glEnd();
	glPopAttrib();
	glPopMatrix();
	
}
