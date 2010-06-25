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
		current.pop_front();
	}
	
}


void IKCharacter::setTarget( int which_leaf, ofxVec3f pos )
{
	leaf_targets[which_leaf] = pos;
}



ofxQuaternion IKCharacter::getRotationForParentBone( int bone_id, ofxVec3f new_bone_pos_world, ofxVec3f new_parent_pos_world )
{
	// get the bone and parent bone's core absolute (==world) translations
	CalCoreSkeleton* core_skel = skeleton->getCoreSkeleton();
	int parent_id = core_skel->getCoreBone( boneId )->getParentId();
	CalVector old_bone_pos_world = core_skel->getCoreBone( bone_id )->getTranslationAbsolute();
	CalVector old_parent_pos_world = core_skel->getCoreBone( parent_id )->getTranslationAbsolute();
	ofxVec3f old_bone_p( old_bone_pos_world.x, old_bone_pos_world.y, old_bone_pos_world.z );
	ofxVec3f old_parent_p( old_parent_pos_world.x, old_parent_pos_world.y, old_parent_pos_world.z );
	
	// get a direction vector for each 
	ofxVec3f old_dir = old_bone_p - old_parent_p;
	ofxVec3f new_dir = new_bone_pos_world - new_parent_pos_world;
	
	// rotate from one to the other
	ofxQuaternion rot;
	rot.makeRotate( old_dir, new_dir );
	
	// return
	return rot;
}
