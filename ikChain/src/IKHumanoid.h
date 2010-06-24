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
#include "Cal3DModel.h"

class IKHumanoid
{
public:
	IKHumanoid() : root_pos( 0, 0 ) {
		arm_target_pos[0].set( -1, 1, 0 ); arm_target_pos[1].set( 1, 1, 0 );
		leg_target_pos[0].set( -1, -1, 0 ); leg_target_pos[0].set( 1, -1, 0 );
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
	void setRootPos( ofxVec3f new_root ) { root_pos = new_root; }
	void setTargetPos( Component which, ofxVec3f new_target ) { getTargetPosFor( which ).set( new_target ); }
	ofxVec3f getTargetPos( Component which ) { return getTargetPosFor( which ); }
	
	// draw
	void draw( int x, int y );
	void dump();
	
	// how many bones?
	int getNumBones( Component which ) { return getBonesFor( which ).size(); }
	IKBone& getBone( Component which, int i ) { assert( i < getNumBones( which ) ); return getBonesFor(which).at(i); }

	// load scale, structure, rest pose from Cal3DModel
	void toCal3DModel( Cal3DModel& c );
	// push current pose to the given Cal3DModel
	void fromCal3DModel( Cal3DModel& c, float scale );
	
private:
	// set current position as rest position
	void setCurrentAsRest();
	
	ofxVec3f& getTargetPosFor( Component which );
	ofxVec3f& getRootPosFor( Component which );
	ofxQuaternion getStartAngleFor( Component which );
	vector<IKBone>& getBonesFor( Component which );
	
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
	vector<ofxVec3f> toCartesianSpace( Component which );
	
	/// update the bones[] vector from the given bone positions
	/// doesn't update or check lengths
	void fromCartesianSpace( Component which, vector<ofxVec3f>& bone_positions );
	
	
	/// solve a simple chain
	void solveSimpleChain(const vector<IKBone>& bones, 
						  vector<ofxVec3f>& bone_positions, 
						  const ofxVec3f& target_pos,
						  bool set_target );
	
	//ofxVec3f root_pos;
	//ofxVec3f target_pos,
	// root_pos == hip_pos
	ofxVec3f root_pos;

	ofxVec3f arm_target_pos[2];
	ofxVec3f leg_target_pos[2];
	ofxVec3f head_target_pos;
	
};




