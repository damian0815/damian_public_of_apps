/*
 *  IKHumanoid.h
 *  emptyExample
 *
 *  Created by damian on 20/06/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "ofxVectorMath.h"
#include "IKBone.h"
#include <assert.h>

class IKHumanoid
{
public:
	IKHumanoid() : root_pos( 0, 0 ) {
		arm_target_pos[0].set( -1, 1 ); arm_target_pos[1].set( 1, 1 );
		leg_target_pos[0].set( -1, -1 ); leg_target_pos[0].set( 1, -1 );
	};

	// scale is a sizing for the skeleton, == 1 head length
	// num_spine_bones sets the number of spine bones (bones between hip and neck)
	void setup( float scale=1.0f, int num_spine_bones = 2 );
	
	// solve using relaxation method from current pose to new pose
	void solve( int iterations = 10);
	
	// reset to rest position
	void resetToRest();
	
	// set root/target positions
	typedef enum _Component{ C_ARM_L, C_ARM_R, C_LEG_L, C_LEG_R, C_SPINE } Component;
	void setRootPos( ofxVec2f new_root ) { root_pos = new_root; }
	void setTargetPos( Component which, ofxVec2f new_target ) { getTargetPosFor( which ).set( new_target ); }
	
	// draw
	void draw( int x, int y );
	void dump();
	
	// how many bones?
	int getNumBones( Component which ) { return getBonesFor( which ).size(); }
	IKBone& getBone( Component which, int i ) { assert( i < getNumBones( which ) ); return getBonesFor(which).at(i); }
	
private:
	
	ofxVec2f& getTargetPosFor( Component which );
	vector<IKBone>& getBonesFor( Component which );
	ofxVec2f& getRootPosFor( Component which );
	float getStartAngleFor( Component which );
	
	vector<IKBone> arms[2];
	vector<IKBone> legs[2];
	// spine consists of:
	// hip==leg branch, <n> spine bones, neck==arm branch, head
	int spine_leg_branch;
	int spine_arm_branch;
	vector<IKBone> spine;
	
	/// return a list of cartesian space coordinates for the skeleton
	/// returns a vector of (bones.size()+1) 2d coordinates;
	/// result[0] is the root pos
	/// result[bones.size()] is the last bone's endpoint
	vector<ofxVec2f> toCartesianSpace( Component which );
	
	/// update the bones[] vector from the given bone positions
	/// doesn't update or check lengths
	void fromCartesianSpace( Component which, vector<ofxVec2f>& bone_positions );
	
	
	/// solve a simple chain
	void solveSimpleChain(const vector<IKBone>& bones, 
						  vector<ofxVec2f>& bone_positions, 
						  const ofxVec2f& target_pos,
						  bool set_target );
	
	//ofxVec3f root_pos;
	//ofxVec3f target_pos,
	// root_pos == hip_pos
	ofxVec2f root_pos;
	ofxVec2f arm_target_pos[2];
	ofxVec2f leg_target_pos[2];
	ofxVec2f head_target_pos;
	
};




