/*
 *  IKCharacter.h
 *  ikChain
 *
 *  Created by damian on 25/06/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "cal3d.h"
#include <vector>
using namespace std;
#include "ofxVectorMath.h"

class IKCharacter
{
public:
	void setup( CalSkeleton* cal_skel );

	// add a knee helper affecting the given bone with the given weight 0..1, parented to the given parent
	void addKneeHelper( string bone, string parent, float weight );

	void draw( float scale );
	
	/// get/set ik target handles
	void setTarget( int which_leaf, ofxVec3f target );
	ofxVec3f getTarget( int which_leaf ) const { CalVector t = (*leaf_targets.find(which_leaf)).second; return ofxVec3f( t.x, t.y, t.z ); }

	/// get the leaf id
	int getLeafId( int index ) { return leaf_bones.at(index); }
	
	void resetToRest();
	void solve( int iterations );
	
	
	
	void rotateBone( int id, ofxVec3f axis, float angle );
	

private:
	/// get world positions from Cal3D skeleton
	void pullWorldPositions();
	/// push world positions to Cal3D skeleton
	void pushWorldPositions();
	
	float getWeightCentre( int id ) { return (*weight_centres.find(id)).second; }
	float getBoneLength( int id ) { return (*bone_lengths.find(id)).second; }
	
	/// return a rotation that will rotate the parent of bone_id in such a way as 
	/// to bring it to the same orientation as described by the direction vector from new_parent_pos_world
	/// to new_bone_pos_world
	ofxQuaternion getRotationForParentBone( int bone_id, CalVector new_bone_pos_world, CalVector new_parent_pos_world );
	
	/// draw from the given bone id down
	void draw( int bone_id, float scale );
	
	vector<int> leaf_bones;
	map<int,CalVector> leaf_targets; // map from leaf bone ids to targets
	map<int,float> bone_lengths;    // map from bone ids to lengths
	map<int,float> weight_centres;  // map from bone ids to weight centres; 0 means pin to parent
	
	CalSkeleton* skeleton;
	
	void swapWorldPositionHandedness();
	map<int,CalVector> world_positions;

	
};

/*


class IKBone
{
public:
	IKBone( IKBone* _parent ): parent(_parent) {};
	
	void addChild( IKBone* child ) { children.push_back( child ); }
	
	/// return a matrix to transform from world-space to 
	ofxMatrix4x4 getWorldToBone();
	ofxQuaternion getAbsolutePosition() { return offset*getWorldToParent(); }
	
private:
	
	// in parent-space
	ofxQuaternion rotation;
	// in parent-space
	ofxVec3f offset;
	
	// in parent space
	ofxQuaternion rest_rotation;
	
	IKBone* parent;
	vector<IKBone*> children;
	
};


*/
