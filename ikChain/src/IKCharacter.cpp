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

int IKCharacter::debug_bone = 12;

#include "ofMain.h"

//#define SWAP_MATRIX_HANDEDNESS
//#define INVERT_QUAT_W
//#define INVERT_QUAT_AXIS

//#define MATRIX_SWAP_YZ
//#define MATRIX_SWAP_XZ
#define MATRIX_SWAP_XY

//#define VECTOR_OD_ND_SWAP_XZ
//#define VECTOR_OD_ND_SWAP_XY
//#define VECTOR_OD_ND_SWAP_YZ
//#define VECTOR_OD_ND_INVERT_X
//#define VECTOR_OD_ND_INVERT_Y
//#define VECTOR_OD_ND_INVERT_Z

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
			current.insert( current.end(), children.begin(), children.end() );
		}
		
		// store length
		// default non-zero
		bone_lengths[current.front()] = 0.0001f;
		if ( check->getParentId()!= -1 )
		{
			CalCoreBone* parent = skeleton->getCoreSkeleton()->getCoreBone( check->getParentId() );
			CalVector delta = check->getTranslationAbsolute() - parent->getTranslationAbsolute();
			bone_lengths[check->getParentId()] = delta.length();
		}
	
		// cached rotations
		debug_cached_rotations[current.front()] = CalQuaternion();
		
		current.pop_front();
	}

	// set all weights to default
	vector<CalCoreBone*> all_bones = skeleton->getCoreSkeleton()->getVectorCoreBone();
	for( int i=0; i<all_bones.size() ;i++ )
	{
		weight_centres[all_bones[i]->getId()] = 0.5f;
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
	/*for( int i=0; i<all_bones.size() ;i++ )
	{
		CalCoreBone* bone = all_bones[i];
		if ( bone->getParentId() == -1 )
			continue;
		CalCoreBone* parent = skeleton->getCoreSkeleton()->getCoreBone( bone->getParentId() );
		if ( parent->getListChildId().size() >1 )
			weight_centres[bone->getId()] = 1;
	}*/
	
	// fetch world positions from Cal3D model
	pullWorldPositions();
	
	// set targets
	for ( int i=0; i<leaf_bones.size(); i++ )
	{
		CalVector p = world_positions[leaf_bones[i]];
		setTarget( leaf_bones[i], ofxVec3f(p.x,p.y,p.z) );
	}
	
	// 
	setupMagicIgnoringRotationOffsets();
	
}


void IKCharacter::setTarget( int which_leaf, ofxVec3f pos )
{
	CalVector p(pos.x, pos.y, pos.z );
	leaf_targets[which_leaf] = p;
}

void IKCharacter::pullWorldPositions( int root_id, int leaf_id )
{
	deque<int> queue;
	if ( leaf_id == -1 )
		queue.insert( queue.begin(), leaf_bones.begin(), leaf_bones.end() );
	else
		queue.push_back( leaf_id );

	// work backwards up the tree
	while ( !queue.empty() )
	{
		int id = queue.front();
		queue.pop_front();
		CalBone* bone = skeleton->getBone(id);
		world_positions[id] = bone->getTranslationAbsolute();
		int parent_id = bone->getCoreBone()->getParentId();
		if ( parent_id != -1 && id != root_id )
			queue.push_back( parent_id );
	}
	/*
	vector<CalBone*>& bones = skeleton->getVectorBone();
	// for each
	for ( int i=0; i<bones.size(); i++) 
	{
		int id= bones[i]->getCoreBone()->getId();
		world_positions[id] = bones[i]->getTranslationAbsolute();
	}*/
	
	
}


void quatToEuler(const ofxQuaternion& quat, double *rotx,  double *roty, double *rotz)
{
	double sqw;
	double sqx;
	double sqy;
	double sqz;
	
	double rotxrad;
	double rotyrad;
	double rotzrad;
	
	sqw = quat.w() * quat.w();
	sqx = quat.x() * quat.x();
	sqy = quat.y() * quat.y();
	sqz = quat.z() * quat.z();
	
	rotxrad = (double)atan2l(2.0 * ( quat.y() * quat.z() + quat.x() * quat.w() ) , ( -sqx - sqy + sqz + sqw ));
	rotyrad = (double)asinl(-2.0 * ( quat.x() * quat.z() - quat.y() * quat.w() ));
	rotzrad = (double)atan2l(2.0 * ( quat.x() * quat.y() + quat.z() * quat.w() ) , (  sqx - sqy - sqz + sqw ));
	
	*rotx = rotxrad * RAD_TO_DEG;
	*roty = rotyrad * RAD_TO_DEG;
	*rotz = rotzrad * RAD_TO_DEG;
	
	return;
}


void getQuaternionToRotateVecToVec( ofxQuaternion& q_out, ofxVec3f v1, ofxVec3f v2,  bool unit_length_vectors = false)
{
	ofxVec3f c = v1.cross(v2);
	ofxVec4f q;
	if (unit_length_vectors) {
		q = ofxVec4f( 1.0f + v1.dot(v2), c.x, c.y, c.z );
	} else {
		q = ofxVec4f(sqrtf( v1.lengthSquared() * v2.lengthSquared()) + v1.dot(v2),
					 c.x, c.y, c.z );
	}
	q.normalize();
	
	q_out.set( q );
}


/** Make a rotation Quat which will rotate vec1 to vec2
 
 This routine uses only fast geometric transforms, without costly acos/sin computations.
 It's exact, fast, and with less degenerate cases than the acos/sin method.
 
 For an explanation of the math used, you may see for example:
 http://logiciels.cnes.fr/MARMOTTES/marmottes-mathematique.pdf
 
 @note This is the rotation with shortest angle, which is the one equivalent to the
 acos/sin transform method. Other rotations exists, for example to additionally keep
 a local horizontal attitude.
 
 @author Nicolas Brodu
 */
void getQuaternionToRotateVecToVec2( ofxQuaternion& q_out, const ofxVec3f& from, const ofxVec3f& to ) {
	
	// This routine takes any vector as argument but normalized
	// vectors are necessary, if only for computing the dot product.
	// Too bad the API is that generic, it leads to performance loss.
	// Even in the case the 2 vectors are not normalized but same length,
	// the sqrt could be shared, but we have no way to know beforehand
	// at this point, while the caller may know.
	// So, we have to test... in the hope of saving at least a sqrt
	ofxVec3f sourceVector = from;
	ofxVec3f targetVector = to;
	
	float fromLen2 = from.lengthSquared();
	float fromLen;
	// normalize only when necessary, epsilon test
	if ((fromLen2 < 1.0 - 1e-7) || (fromLen2 > 1.0 + 1e-7)) {
		fromLen = sqrt(fromLen2);
		sourceVector /= fromLen;
	} else fromLen = 1.0;
	
	float toLen2 = to.lengthSquared();
	// normalize only when necessary, epsilon test
	if ((toLen2 < 1.0 - 1e-7) || (toLen2 > 1.0 + 1e-7)) {
		float toLen;
		// re-use fromLen for case of mapping 2 vectors of the same length
		if ((toLen2 > fromLen2 - 1e-7) && (toLen2 < fromLen2 + 1e-7)) {
			toLen = fromLen;
		} else toLen = sqrt(toLen2);
		targetVector /= toLen;
	}
	
	
	// Now let's get into the real stuff
	// Use "dot product plus one" as test as it can be re-used later on
	double dotProdPlus1 = 1.0 + sourceVector.dot(targetVector);
	
	float _v[4];
	
	// Check for degenerate case of full u-turn. Use epsilon for detection
	if (dotProdPlus1 < 1e-7) {
		
		// Get an orthogonal vector of the given vector
		// in a plane with maximum vector coordinates.
		// Then use it as quaternion axis with pi angle
		// Trick is to realize one value at least is >0.6 for a normalized vector.
		if (fabs(sourceVector.x) < 0.6) {
			const double norm = sqrt(1.0 - sourceVector.x * sourceVector.x);
			_v[0] = 0.0;
			_v[1] = sourceVector.z / norm;
			_v[2] = -sourceVector.y / norm;
			_v[3] = 0.0;
		} else if (fabs(sourceVector.y) < 0.6) {
			const double norm = sqrt(1.0 - sourceVector.y * sourceVector.y);
			_v[0] = -sourceVector.z / norm;
			_v[1] = 0.0;
			_v[2] = sourceVector.x / norm;
			_v[3] = 0.0;
		} else {
			const double norm = sqrt(1.0 - sourceVector.z * sourceVector.z);
			_v[0] = sourceVector.y / norm;
			_v[1] = -sourceVector.x / norm;
			_v[2] = 0.0;
			_v[3] = 0.0;
		}
	}
	
	else {
		// Find the shortest angle quaternion that transforms normalized vectors
		// into one other. Formula is still valid when vectors are colinear
		const double s = sqrt(0.5 * dotProdPlus1);
		const ofxVec3f tmp = sourceVector.getCrossed(targetVector) / (2.0 * s);
		_v[0] = tmp.x;
		_v[1] = tmp.y;
		_v[2] = tmp.z;
		_v[3] = s;
	}
	
	q_out.set( _v[0], _v[1], _v[2], _v[3] );
}



/// calculate a rotation to bring the current direction vector between parent and bone
/// to the given new direction vector (non-normalized)
ofxQuaternion IKCharacter::getRotationForParentBone( int bone_id, CalVector new_parent_to_bone_direction )
{
	CalCoreSkeleton* core_skel = skeleton->getCoreSkeleton();
	CalBone* bone = skeleton->getBone( bone_id );
	int parent_id = core_skel->getCoreBone( bone_id )->getParentId();
	CalBone* parent_bone = skeleton->getBone( parent_id );
	int parent_parent_id = parent_bone->getCoreBone()->getParentId();
	// don't rotate the base state
	if ( parent_parent_id == -1 )
		return ofxQuaternion();


	// new_parent_to_bone_direction is currently in world-space
	// we need to bring it into the space of the parent bone
	CalQuaternion bone_space_rot = bone->getRotationBoneSpace();
	bone_space_rot.invert();
	new_parent_to_bone_direction *= bone_space_rot;

	CalVector old_dir = bone->getTranslation();
	CalVector new_dir = new_parent_to_bone_direction;
	old_dir.normalize();
	new_dir.normalize();
	
	
	// rotate from one to the other
	ofxQuaternion rot;
	ofxVec3f od( old_dir.x, old_dir.y, old_dir.z );
	ofxVec3f nd( new_dir.x, new_dir.y, new_dir.z );
#ifdef VECTOR_OD_ND_INVERT_Y
	od.y = -od.y;
	nd.y = -nd.y;
#endif
#ifdef VECTOR_OD_ND_INVERT_X
	od.x = -od.x;
	nd.x = -nd.x;
#endif
#ifdef VECTOR_OD_ND_INVERT_Z
	od.z = -od.z;
	nd.z = -nd.z;
#endif
#ifdef VECTOR_OD_ND_SWAP_YZ
	float swap = od.y;
	od.y = od.z;
	od.z = swap;
	swap = nd.y;
	nd.y = nd.z;
	nd.z = swap;
#endif
#ifdef VECTOR_OD_ND_SWAP_XZ
	float swap = od.x;
	od.x = od.z;
	od.z = swap;
	swap = nd.x;
	nd.x = nd.z;
	nd.z = swap;
#endif
#ifdef VECTOR_OD_ND_SWAP_XY
	float swap = od.y;
	od.y = od.x;
	od.x = swap;
	swap = nd.y;
	nd.y = nd.x;
	nd.x = swap;
#endif
	rot.makeRotate( od, nd );
	// the result needs to be reflected to make it work with cal3d
	//rot.set( rot.x(), rot.y(), rot.z(), rot.w() );

	
	{
		double rot_eux, rot_euy, rot_euz;
		quatToEuler( rot, &rot_eux, &rot_euy, &rot_euz );
		/*
		 CalVector old_bone_pos_world = bone->getTranslationAbsolute();
		 CalVector old_parent_pos_world = parent_bone->getTranslationAbsolute();
		 // get a direction vector for each 
		 CalVector old_dir = old_bone_pos_world - old_parent_pos_world;
		 old_dir.normalize();
		 */

		
		if ( parent_id == debug_bone )
		{

			ofxQuaternion newrot;
			getQuaternionToRotateVecToVec2( newrot, od, nd );
			
			double rot_n_eux, rot_n_euy, rot_n_euz;
			quatToEuler( newrot, &rot_n_eux, &rot_n_euy, &rot_n_euz );
			printf("old_dir %6.3f %6.3f %6.3f %s\n"
				   "new_dir %6.3f %6.3f %6.3f\n"
				   "rotate  %6.3f %6.3f %6.3f\n"
				   "rotnew  %6.3f %6.3f %6.3f\n", old_dir.x, old_dir.y, old_dir.z, parent_bone->getCoreBone()->getName().c_str(),
				   new_dir.x, new_dir.y, new_dir.z,
				   rot_eux, rot_euy, rot_euz,
				   rot_n_eux, rot_n_euy, rot_n_euz);
			
			/*rot.makeRotate(rot_eux*DEG_TO_RAD, ofxVec3f(1,0,0), 
						   rot_euy*DEG_TO_RAD, ofxVec3f(0,-1,0), 
						   rot_euz*DEG_TO_RAD, ofxVec3f(0,0,1) );*/
			
			//rot = newrot;
			//rot = rot.inverse();
		}	
		
		//rot = newrot;
		
		//rot *= ofxQuaternion( PI, ofxVec3f( 0, 1, 0 ) );
		
	}	

	
	
	// return
	return rot.inverse();
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


void IKCharacter::setupMagicIgnoringRotationOffsets()
{
	pullWorldPositions();

	// start at the roots
	vector<int> roots = skeleton->getCoreSkeleton()->getVectorRootCoreBoneId();
	deque<int> parent_queue;
	parent_queue.insert( parent_queue.begin(), roots.begin(), roots.end() );
	while ( !parent_queue.empty() )
	{
		// get parent bone id
		int parent_id = parent_queue.front();
		parent_queue.pop_front();
		// get the bone
		CalBone* parent_bone = skeleton->getBone( parent_id );
		// get child list
		list<int> children = parent_bone->getCoreBone()->getListChildId();
		// no children?
		if ( children.size() == 0 )
			continue;
		
		// calculate a result for each child
		vector<ofxQuaternion> results;
		for ( list<int>::iterator it = children.begin();
			 it != children.end();
			 it++ )
		{
			// get new world positions
			CalVector b,p;
			int child_id = *it;
			b = world_positions[child_id];
			p = world_positions[parent_id];
			// get the rotation
			ofxQuaternion rot = getRotationForParentBone( child_id, (b-p) );
			
			// store as average
			results.push_back( rot );
		}
		// slerp
		vector<ofxQuaternion> other_results;
		while ( results.size() > 1 )
		{
			other_results.clear();
			for ( int i=0; i<results.size(); i+=2 )
			{
				other_results.push_back( ofxQuaternion() );
				other_results.back().slerp( 0.5f, results[0], results[1] );
			}
			results.swap( other_results );
		}
		ofxQuaternion rot( results[0] );
		CalQuaternion rot_cal( rot.x(), rot.y(), rot.z(), rot.w() );
		rot_cal.invert();
		magic_ignoring_rotation_offset[parent_id] = rot_cal;
		
		// add all children
		for ( list<int>::iterator it =children.begin() ;it != children.end(); ++it )
		{
			parent_queue.push_back( *it );
		}
		
	}
}


void IKCharacter::pushWorldPositions( bool re_solve )
{
	
	// start at the roots
	vector<int> roots = skeleton->getCoreSkeleton()->getVectorRootCoreBoneId();

	// populate the parent queue
	deque<int> parent_queue;
	parent_queue.insert( parent_queue.begin(), roots.begin(), roots.end() );
	while ( !parent_queue.empty() )
	{
		// get parent bone id
		int parent_id = parent_queue.front();
		parent_queue.pop_front();
		// get the bone
		CalBone* parent_bone = skeleton->getBone( parent_id );
		// get child list
		list<int> children = parent_bone->getCoreBone()->getListChildId();
		// no children?
		if ( children.size() == 0 )
			continue;
				
		// calculate a result for each child
		vector<ofxQuaternion> results;
		for ( list<int>::iterator it = children.begin();
			 it != children.end();
			 it++ )
		{
			// get new world positions
			CalVector b,p;
			int child_id = *it;
			b = world_positions[child_id];
			p = world_positions[parent_id];
			// get the rotation
			ofxQuaternion rot = getRotationForParentBone( child_id, (b-p) );
			
			
#ifdef SWAP_MATRIX_HANDEDNESS
			// FLIP THE ROTATION FROM LEFTHANDED to RIGHTHANDED gnashgnashgnash
			ofxMatrix4x4 flippy( rot );
			swapHandedness( flippy );
			rot.set( flippy );
#endif
			
#ifdef INVERT_QUAT_W
			rot.set( rot.x(), rot.y(), rot.z(), -rot.w() );
#endif
#ifdef INVERT_QUAT_AXIS
			rot.set( -rot.x(), -rot.y(), -rot.z(), rot.w() );
#endif
			
			// store as average
			results.push_back( rot );
		}
		// slerp
		vector<ofxQuaternion> other_results;
		while ( results.size() > 1 )
		{
			other_results.clear();
			for ( int i=0; i<results.size(); i+=2 )
			{
				other_results.push_back( ofxQuaternion() );
				other_results.back().slerp( 0.5f, results[0], results[1] );
			}
			results.swap( other_results );
		}
		ofxQuaternion rot( results[0] );
		CalQuaternion rot_cal( rot.x(), rot.y(), rot.z(), rot.w() );
			
/*		// don't apply if has siblings?
		int parent_parent_id = parent_bone->getCoreBone()->getParentId();
		if ( parent_parent_id != -1 && 
			skeleton->getCoreSkeleton()->getCoreBone( parent_parent_id )->getListChildId().size() > 1 )
		{
			// nothing
		}
		else*/
		{
			
			// apply the rotation
			CalBone* bone_to_set = parent_bone;
			rot_cal *= magic_ignoring_rotation_offset[bone_to_set->getCoreBone()->getId()];
			bone_to_set->setRotation( bone_to_set->getCoreBone()->getRotation()*rot_cal);
			//printf("setting a rotation for %s\n", bone_to_set->getCoreBone()->getName().c_str() );
			// calculates children also
			bone_to_set->calculateState();
			debug_cached_rotations[bone_to_set->getCoreBone()->getId()] = rot_cal;
			
			// re-solve?
			if ( re_solve )
			{
				if ( children.size() > 0 )
				{
					// get a tree going from this node down to leaf, and re-solve for just that tree
					int root_id = children.front();
					
					int leaf_id = root_id;
					while( true )
					{
						list<int>& next_children = skeleton->getCoreSkeleton()->getCoreBone( leaf_id )->getListChildId();
						if ( next_children.size() > 1 )
						{
							leaf_id = -1;
							break;
						}
						else if ( next_children.size() == 0 )
							// found leaf
							break;
						else
							// exactly one child
							leaf_id = next_children.front();
					}
					// re-solve if we should
					if ( leaf_id != -1 )
					{
						/*printf("re-solving from %s up to %s\n", 
							   skeleton->getCoreSkeleton()->getCoreBone( leaf_id )->getName().c_str(),
							   skeleton->getCoreSkeleton()->getCoreBone( root_id )->getName().c_str() );*/
						pullWorldPositions( root_id, leaf_id );
						solve( 2, root_id, leaf_id );
					}
				}
			}
		}
		
		// add all children
		for ( list<int>::iterator it =children.begin() ;it != children.end(); ++it )
		{
			parent_queue.push_back( *it );
		}
	}

	skeleton->calculateState();

}


void IKCharacter::resetToRest()
{
	vector<int> roots = skeleton->getCoreSkeleton()->getVectorRootCoreBoneId();
	for ( int i=0; i<roots.size(); i++ )
	{
		skeleton->getBone( roots[i] )->setCoreStateRecursive();
	}
	pullWorldPositions();
}



void IKCharacter::solve( int iterations, int root_id, int leaf_id )
{
	while ( iterations>0 )
	{
		// start at leaf nodes
		deque<int> queue;
		
		// push leaf bones to target positions
		for ( int i=0; i<leaf_bones.size(); i++ )
		{
			// if we have a leaf id to use, skip all other leaves
			if ( leaf_id != -1 && leaf_id != leaf_bones[i] )
				continue;
			
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
		
		// queue to handle branching
		deque< int > branch_queue;
		while ( !queue.empty() || !branch_queue.empty() )
		{
			// if main queue is empty then we are ready to deal with branches
			if ( queue.empty() )
			{
				queue.push_back( branch_queue.front() );
				branch_queue.pop_front();
				continue;
			}
			
			int next_id = queue.front();
			queue.pop_front();
			// bail out if we should
			if ( root_id != -1 && next_id == root_id )
				continue;
			
			CalBone* bone = skeleton->getBone( next_id );
			list<int>& children = bone->getCoreBone()->getListChildId();
			// is this a branch?
			if ( children.size()>1 )
			{
				// still other children to process -- push to branch queue
				if ( !queue.empty() )
				{
					branch_queue.push_back( next_id );
					continue;
				}
				
				// otherwise, process branch here
				// if we're here, then all the children of this branch have been processed already
				int parent_id = bone->getCoreBone()->getParentId();
				if ( parent_id != -1 )
				{
					//printf("averaging %lu positions for %s wc %f: ", children.size(), bone->getCoreBone()->getName().c_str(), getWeightCentre( next_id ) );
					// fetch all child positions
					vector<CalVector> results;
					results.insert( results.begin(), children.size(), world_positions[next_id] );
					int count=0;
					for ( list<int>::iterator it = children.begin(); it != children.end(); ++it,++count ) 
					{
						
						// child_pos
						CalVector& b_c = world_positions[*it];
						// current pos
						CalVector& b_p = results[count];

						// now, the bone is the wrong length. correct its length to fulfil size constraint.
						CalVector delta = b_c - b_p;
						float length = delta.length();
						length = max( 0.00001f, length );
						// pointing from parent to child
						CalVector direction = delta/length;
						
						CalCoreBone* child_bone = skeleton->getCoreSkeleton()->getCoreBone( *it );
						CalVector rest_delta = bone->getCoreBone()->getTranslationAbsolute() - child_bone->getTranslationAbsolute();
						float desired_length = rest_delta.length();
						float delta_length = desired_length - length;
						
						// balance according to weight_centre
						float weight_centre = getWeightCentre(next_id);
						
						// move
						b_c += weight_centre * delta_length * direction;
						b_p -= (1.0f-weight_centre) * delta_length * direction;
						
						//printf("%s %f (%f %f %f), ", child_bone->getName().c_str(), delta_length, b_p.x, b_p.y, b_p.z );
						
					}
					//printf("\n");
					
					// now average
					CalVector average;
					for ( int i=0; i<results.size(); i++ )
					{
						average += results[i];
					}
					average /= results.size();

					// store
					world_positions[next_id] = average;
					
					// add parent
					queue.push_back( parent_id );
				}				
				
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
				float desired_length = getBoneLength(next_id);
				CalVector delta = b_c - b_p;
				float length = delta.length();
				length = max( 0.00001f, length );
				length = min( desired_length*1.5f, length );
				// pointing from parent to child
				CalVector direction = delta/length;
				
				float delta_length = desired_length - length;
				
				// balance according to weight_centre
				float weight_centre = getWeightCentre(next_id);
				
				// move
				b_c += weight_centre * delta_length * direction;
				b_p -= (1.0f-weight_centre) * delta_length * direction;
				
				
				// add parent
				int parent_id = bone->getCoreBone()->getParentId();
				if ( parent_id != -1 )
					queue.push_back( parent_id );
			}
		}	
		
		iterations--;
	}
	
	
}





void IKCharacter::draw( int bone_id, float scale )
{
	CalBone* bone = skeleton->getBone( bone_id );
	int parent_id = bone->getCoreBone()->getParentId();
	if ( parent_id != -1 )
	{
		// current
		CalBone* parent = skeleton->getBone( parent_id );
		glBegin( GL_LINES );
		CalVector v = parent->getTranslationAbsolute();
		v*=scale;
		CalVector c = v;
		glVertex3f( v.x, v.y, v.z );
		v = bone->getTranslationAbsolute();
		v*=scale;
		c += v;
		c /= 2.0f;
		glVertex3f( v.x, v.y, v.z );
		glEnd();

		// world
		glPushAttrib( GL_CURRENT_BIT );
		glColor3f( 0.1f, 0.8f, 0.1f );
		glBegin( GL_LINES );
		v = world_positions[parent_id];
		v*=scale;
		glVertex3f( v.x, v.y, v.z );
		v = world_positions[bone_id];
		v*=scale;
		glVertex3f( v.x, v.y, v.z );

		// core
		glColor3f( (parent_id==debug_bone)?1.0f:0.3f, 0.3f, 0.5f );
		v = parent->getCoreBone()->getTranslationAbsolute();
		v*=scale;
		glVertex3f( v.x, v.y, v.z );
		v = bone->getCoreBone()->getTranslationAbsolute();
		v*=scale;
		glVertex3f( v.x, v.y, v.z );
		glEnd();
		

		// draw rotation
		glPushMatrix();
		CalVector root = bone->getCoreBone()->getTranslationAbsolute();
		glTranslatef( root.x, root.y, root.z );
		CalVector rot;
		
		rot.set( 0, 0.1f*scale, 0 );
		rot *= bone->getCoreBone()->getRotationAbsolute();
		ofEnableAlphaBlending();
		glColor4f( 0.2f, 0.2f, 0.8f, 0.2f );
		glBegin( GL_TRIANGLES );
		glVertex3f( 0,0,0 );
		glVertex3f( rot.x, rot.y, rot.z );
		rot *= debug_cached_rotations[bone_id];
		glVertex3f( rot.x, rot.y, rot.z );
		glEnd();
		
		glColor4f( 0.2f, 0.2f, 0.8f, 0.8f );
		rot.set( 0, 0.1f*scale, 0 );
		rot *= bone->getCoreBone()->getRotationAbsolute();
		glBegin( GL_LINES );
		glVertex3f( 0,0,0 );
		glVertex3f( rot.x, rot.y, rot.z );
		rot *= debug_cached_rotations[bone_id];
		glVertex3f( 0,0,0 );
		glVertex3f( rot.x, rot.y, rot.z );
		glEnd();
		
		ofDisableAlphaBlending();
		
		glPopMatrix();
		
		

				   
				   

		CalVector u( 0, 0.1f*scale, 0);
		u *= bone->getRotationAbsolute();
		CalVector r( 0.1f*scale, 0, 0 );
		r *= bone->getRotationAbsolute();
		CalVector f( 0, 0, 0.1f*scale );
		f *= bone->getRotationAbsolute();
		
		
		// right blue
		glPushMatrix();
		root = bone->getTranslationAbsolute();
		glTranslatef( root.x, root.y, root.z );
		glBegin( GL_LINES );
		glColor3f( 0, 0, 1 );
		glVertex3f( 0,0,0 );
		glVertex3f( r.x, r.y, r.z );
		// up red
		glColor3f( 1, 0, 0 );
		glVertex3f( 0,0,0 );
		glVertex3f( u.x, u.y, u.z );
		// forward green
		glColor3f( 0, 1, 0 );
		glVertex3f( 0,0,0 );
		glVertex3f( f.x, f.y, f.z );
		glEnd();
		glPopMatrix();

		// right blue
		glPushMatrix();
		root = world_positions[bone_id];
		glTranslatef( root.x, root.y, root.z );
		glBegin( GL_LINES );
		glColor3f( 0, 0, 1 );
		glVertex3f( 0,0,0 );
		glVertex3f( r.x, r.y, r.z );
		// up red
		glColor3f( 1, 0, 0 );
		glVertex3f( 0,0,0 );
		glVertex3f( u.x, u.y, u.z );
		// forward green
		glColor3f( 0, 1, 0 );
		glVertex3f( 0,0,0 );
		glVertex3f( f.x, f.y, f.z );
		glEnd();
		glPopMatrix();
		
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


void IKCharacter::addKneeHelper( string bone, string parent, float weight )
{
}
