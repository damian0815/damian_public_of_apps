/*
 *  IKHumanoid.cpp
 *  emptyExample
 *
 *  Created by damian on 20/06/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "IKHumanoid.h"
#include "ofMain.h"
#include <assert.h>

static IKHumanoid::Component COMPONENT_LIST[5] = { 
	IKHumanoid::C_SPINE, 
	IKHumanoid::C_ARM_L, 
	IKHumanoid::C_ARM_R, 
	IKHumanoid::C_LEG_L, 
	IKHumanoid::C_LEG_R 
};

void IKHumanoid::setup( float scale, int num_spine_bones )
{
	root_pos = ofxVec2f( 0, 0 );
	// spine
	float spine_joint_length = (1.4f)/float(num_spine_bones);
	for( int i=0; i<num_spine_bones; i++ )
	{
		spine.push_back( IKBone( spine_joint_length*scale, 0, 0.5f ) );
	}
	// head
	spine.push_back( IKBone( 0.9f*scale, 0, 0.5f ) );

	// convert spine to cartesian coordinates
	vector<ofxVec2f> spine_pos = toCartesianSpace( C_SPINE );
	// set head target pos
	setTargetPos( C_SPINE, spine_pos.back() );
	
	// store branch locations
	spine_leg_branch = 0;
	spine_arm_branch = num_spine_bones;

	// arms
	arms[0].resize( 3 );
	arms[1].resize( 3 );
	for ( int i= 0; i<2;i ++ )
	{
		float dir = ((i==0)?(-1.0f):(1.0f));
		vector<ofxVec2f> pos;
		// neck joint
		printf("scale: %f\n", scale );
		pos.push_back( spine_pos[spine_arm_branch] );
		// shoulder
		ofxVec2f delta = ofxVec2f( dir*1.0f, -0.1f )*scale;
		arms[i][0].setLength( delta.length() );
		pos.push_back( pos.back()+delta );
		// upper arm
		delta = ofxVec2f( dir*1.0f, 0.0f )*scale;
		arms[i][1].setLength( delta.length() );
		pos.push_back( pos.back()+delta );
		// lower arm
		delta = ofxVec2f( dir*1.5f, 0.0f )*scale;
		arms[i][2].setLength( delta.length() );
		pos.push_back( pos.back()+delta );
		// store
		Component which = (i==0?C_ARM_L:C_ARM_R);
		fromCartesianSpace( which, pos );
		setTargetPos( which, pos.back() );
/*		printf("target pos for arms is: [%5.1f %5.1f] (%5.1f %5.1f) from (%5.1f %5.1f)\n", 
			   arm_target_pos[i].x, arm_target_pos[i].y, 
			   getTargetPosFor( which ).x, getTargetPos( which ).y, 
			   pos.back().x, pos.back().y );*/
	}
	
	// legs
	legs[0].resize( 3 );
	legs[1].resize( 3 );
	for ( int i=0; i<2; i++ )
	{
		float dir = ((i==0)?(-1.0f):(1.0f));
		vector<ofxVec2f> pos;
		// hip joint
		pos.push_back( spine_pos[spine_leg_branch] );
		// thigh
		ofxVec2f delta = ofxVec2f( dir*0.5f, -0.4f )*scale;
		legs[i][0].setLength( delta.length() );
		pos.push_back( pos.back()+delta );
		// knee
		delta = ofxVec2f( dir*-0.05f, -1.6f )*scale;
		legs[i][1].setLength( delta.length() );
		pos.push_back( pos.back()+delta );
		// foot
		delta = ofxVec2f( dir*-0.05f, -2.2f )*scale;
		legs[i][2].setLength( delta.length() );
		pos.push_back( pos.back()+delta );
		// store
		Component which = (i==0?C_LEG_L:C_LEG_R);
		fromCartesianSpace( which, pos );
		setTargetPos( which, pos.back() );
	}
	
	// set rest angles
	for ( int i=0; i<5; i++ )
	{
		Component which = COMPONENT_LIST[i];
		vector<IKBone>& bones = getBonesFor( which );
		for ( int j=0; j<bones.size() ;j++ )
		{
			bones[j].setRestAngle( bones[j].getAngle() );
		}
	}
	
	// set angle constraints for particular joints
	// shoulders only move a very small distance
	float angle = arms[0][0].getAngle();
	arms[0][0].setRestAngle( angle, -PI/16, PI/16 );
	angle = arms[1][0].getAngle();
	arms[1][0].setRestAngle( angle, -PI/16, PI/16 );
	// waist to thigh doesn't move
	angle = legs[0][0].getAngle();
	legs[0][0].setRestAngle( angle, 0, 0 );
	angle = legs[1][0].getAngle();
	legs[1][0].setRestAngle( angle, 0, 0 );
	// waist to spine doesn't move
	angle = spine[0].getAngle();
	spine[0].setRestAngle( angle, 0, 0 );

	// anchor the waist
	getBone( C_SPINE, 0 ).setWeightCentre( 1 );
	
	dump();
}

void IKHumanoid::dump()
{
	// dump
	vector<ofxVec2f> pos = toCartesianSpace( C_SPINE );
	printf("Spine: \n");
	for ( int i=0; i<pos.size(); i++ )
	{
		printf(" (%5.1f %5.1f)", pos[i].x, pos[i].y );
	}
	pos = toCartesianSpace( C_ARM_L );
	printf("\nArm_L: \n");
	for ( int i=0; i<pos.size(); i++ )
	{
		printf(" (%5.1f %5.1f)", pos[i].x, pos[i].y );
	}
	pos = toCartesianSpace( C_ARM_R );
	printf("\nArm_R: \n");
	for ( int i=0; i<pos.size(); i++ )
	{
		printf(" (%5.1f %5.1f)", pos[i].x, pos[i].y );
	}
	pos = toCartesianSpace( C_LEG_L );
	printf("\nLeg_L: \n");
	for ( int i=0; i<pos.size(); i++ )
	{
		printf(" (%5.1f %5.1f)", pos[i].x, pos[i].y );
	}
	pos = toCartesianSpace( C_LEG_R );
	printf("\nLeg_R: \n");
	for ( int i=0; i<pos.size(); i++ )
	{
		printf(" (%5.1f %5.1f)", pos[i].x, pos[i].y );
	}

}

void IKHumanoid::solveSimpleChain(const vector<IKBone>& bones, 
								  vector<ofxVec2f>& bone_positions, 
								  const ofxVec2f& target_pos,
								  bool set_target )
{
	// solve using constraint relaxation
	// after http://www.ryanjuckett.com/programming/animation/22-constraint-relaxation-ik-in-2d

	// push the last bone to the target position
	if ( set_target )
		bone_positions[bones.size()] = target_pos;
	for ( int bone_num = bones.size()-1; bone_num >= 0; bone_num-- )
	{
		// child_pos
		ofxVec2f& b_c = bone_positions[bone_num+1];
		// parent pos
		ofxVec2f& b_p = bone_positions[bone_num];
		
		// now, the bone is the wrong length. correct its length to fulfil size constraint.
		ofxVec2f delta = b_c - b_p;
		float length = delta.length();
		length = max( 0.00001f, length );
		// pointing from parent to child
		ofxVec2f direction = delta/length;
		
		float desired_length = bones[bone_num].getLength();
		float delta_length = desired_length - length;
		
		// balance according to weight_centre
		float weight_centre;
		// for the last bone, we move only the start (parent) point
		if ( bone_num == bones.size()-1 )
			weight_centre = 0;
		// for other bones we move points based on weight centre
		else 
			weight_centre = bones[bone_num].getWeightCentre();
		
		// move
		b_c += weight_centre * delta_length * direction;
		b_p -= (1.0f-weight_centre) * delta_length * direction;
	}		
}


void IKHumanoid::solve( int iterations )
{
	// solve using constraint relaxation
	// after http://www.ryanjuckett.com/programming/animation/22-constraint-relaxation-ik-in-2d
	
	
	for ( int i=0; i<iterations; i++ )
	{
		printf("iteration %i\n", i );
		// first put all bones to cartesian space
		vector<ofxVec2f> spine_bone_positions = toCartesianSpace( C_SPINE );
		vector<ofxVec2f> arm_bone_positions[2];
		float arm_branch_angle = (spine_bone_positions[spine_arm_branch+1] - 
								  spine_bone_positions[spine_arm_branch]).angleRad( ofxVec2f( 0, 1 ) );
		arm_bone_positions[0] = toCartesianSpace( C_ARM_L, -arm_branch_angle );
		arm_bone_positions[1] = toCartesianSpace( C_ARM_R, -arm_branch_angle );
		vector<ofxVec2f> leg_bone_positions[2];
		float leg_branch_angle = (spine_bone_positions[spine_leg_branch+1] - 
								  spine_bone_positions[spine_leg_branch]).angleRad( ofxVec2f( 0, 1 ) );
		leg_bone_positions[0] = toCartesianSpace( C_LEG_L, -leg_branch_angle );
		leg_bone_positions[1] = toCartesianSpace( C_LEG_R, -leg_branch_angle );

		// now solve for the spine
		solveSimpleChain( spine, spine_bone_positions, head_target_pos, i==0 );
		
		// next, solve the two arms, setting the neck position to be a weighted 
		// average of spine neck, arm_l neck and arm_r neck calculated positions
		solveSimpleChain( arms[0], arm_bone_positions[0], arm_target_pos[0], i==0 );
		solveSimpleChain( arms[1], arm_bone_positions[1], arm_target_pos[1], i==0 );
		ofxVec2f neck_pos = spine_bone_positions[spine_arm_branch]*0.8f;
		neck_pos += arm_bone_positions[0][0]*0.1f;
		neck_pos += arm_bone_positions[1][0]*0.1f;
		spine_bone_positions[spine_arm_branch] = neck_pos;
		arm_bone_positions[0][0] = neck_pos;
		arm_bone_positions[1][0] = neck_pos;
		
		// last, solve the two legs, setting the hip position to be a weighted 
		// average of spine hip, leg_l hip and leg_r hip
		solveSimpleChain( legs[0], leg_bone_positions[0], leg_target_pos[0], i==0 );
		solveSimpleChain( legs[1], leg_bone_positions[1], leg_target_pos[1], i==0 );
		ofxVec2f hip_pos = spine_bone_positions[spine_leg_branch]*0.5f;
		hip_pos += leg_bone_positions[0][0]*0.25f;
		hip_pos += leg_bone_positions[1][0]*0.25f;
		spine_bone_positions[spine_leg_branch] = hip_pos;
		leg_bone_positions[0][0] = hip_pos;
		leg_bone_positions[1][0] = hip_pos;

		// convert back to angles
		fromCartesianSpace( C_SPINE, spine_bone_positions );
		fromCartesianSpace( C_ARM_L, arm_bone_positions[0], arm_branch_angle );
		fromCartesianSpace( C_ARM_R, arm_bone_positions[1], arm_branch_angle );
		fromCartesianSpace( C_LEG_L, leg_bone_positions[0], leg_branch_angle );
		fromCartesianSpace( C_LEG_R, leg_bone_positions[1], leg_branch_angle );
		
		// constrain angles
		for ( int component = 0; component<5; component++ )
		{
			vector<IKBone>& bones = getBonesFor( COMPONENT_LIST[component] );
			for ( int j=0; j<bones.size(); j++ )
			{
				bones[j].constrainAngle();
			}
		}		
	}
	
	
}


vector<ofxVec2f> IKHumanoid::toCartesianSpace( Component which, float start_angle )
{
	// put all the bones into 2d space
	vector<ofxVec2f> bone_positions;
	// start at the root
	bone_positions.push_back( getRootPosFor( which ) );
	// start pointing up
	ofxVec2f dir( 0,1 );
	dir.rotateRad( start_angle );
	vector<IKBone>& bones = getBonesFor( which );
	for ( int i=0; i<bones.size(); i++ )
	{
		// rotate our direction by the bone's angle
		dir.rotateRad( bones[i].getAngle() );
		// add on direction * length to get the end point of this bone
		ofxVec2f next_pos = bone_positions.back() + dir*bones[i].getLength();
		bone_positions.push_back( next_pos );
	}
	
	return bone_positions;
}

void IKHumanoid::fromCartesianSpace( Component which, vector<ofxVec2f>& bone_positions, float start_angle )
{
	vector<IKBone>& bones = getBonesFor( which );
	assert( bone_positions.size() == bones.size()+1 );
	
	//getRootPosFor(which).set( bone_positions[0] );
	ofxVec2f dir( 0, 1 );
	dir.rotateRad( start_angle );
	for ( int i=0; i<bones.size(); i++ )
	{
		// get bone parent->child delta
		ofxVec2f bone_delta = bone_positions[i+1]-bone_positions[i];
		// convert bone delta to a direction
		bone_delta.normalize();
		// -> angle
		float angle = bone_delta.angleRad( dir );
		bones[i].setAngle( -angle );
		// rotate our direction by the bone's angle (angles are all relative)
		dir.rotateRad( -angle );
	}
	
}


void IKHumanoid::draw( int x, int y )
{
	ofPushMatrix();
	ofTranslate( x, ofGetHeight()-y, 0 );
	ofScale( 1, -1, 1 );
	for ( int i=0; i<5; i++ )
	{
		Component which;
		switch(i)
		{
			case 0:
				which = C_SPINE;
				break;
			case 1:
				which = C_ARM_L;
				break;
			case 2:
				which = C_ARM_R;
				break;
			case 3:
				which = C_LEG_L;
				break;
			case 4:
				which = C_LEG_R;
				break;
			default:
				which = C_SPINE;
				break;
		}
		
		vector<ofxVec2f> bone_positions = toCartesianSpace( which );
	
		ofSetColor( 128, 255, 128 );
		ofxVec2f root_pos = getRootPosFor( which );
		ofCircle( root_pos.x, root_pos.y, 5 );
		ofSetColor( 128, 128, 128 );
		ofBeginShape();
		ofNoFill();
		ofCurveVertex( bone_positions[0].x, bone_positions[0].y );
		for ( int i=0; i<bone_positions.size(); i++ )
		{
			if ( i < bone_positions.size()-1 )
			{
				ofLine( bone_positions[i].x, bone_positions[i].y, 
					   bone_positions[i+1].x, bone_positions[i+1].y );
			}
			ofCurveVertex( bone_positions[i].x, bone_positions[i].y );
		}
		ofCurveVertex( bone_positions[bone_positions.size()-1].x, bone_positions[bone_positions.size()-1].y );
		ofEndShape();
		ofSetColor( 255, 128, 128 );
		ofxVec2f target_pos = getTargetPosFor( which );
		ofCircle(target_pos.x, 
				 target_pos.y, 
				 5 );
	}
	ofPopMatrix();
}


vector<IKBone>& IKHumanoid::getBonesFor( Component which )
{
	switch( which ) 
	{
		case C_SPINE:
			return spine;
		case C_ARM_L:
			return arms[0];
		case C_ARM_R:
			return arms[1];
		case C_LEG_L:
			return legs[0];
		case C_LEG_R:
			return legs[1];
		default:
			return spine;
	}
}

ofxVec2f& IKHumanoid::getTargetPosFor( Component which )
{
	switch( which ) 
	{
		case C_SPINE:
			return head_target_pos;
		case C_ARM_L:
			return arm_target_pos[0];
		case C_ARM_R:
			return arm_target_pos[1];
		case C_LEG_L:
			return leg_target_pos[0];
		case C_LEG_R:
			return leg_target_pos[1];
		default:
			return head_target_pos;
	}
}

ofxVec2f& IKHumanoid::getRootPosFor( Component which )
{
	if ( which == C_SPINE )
		return root_pos;
	else
	{
		vector<ofxVec2f> pos = toCartesianSpace( C_SPINE );
		switch (which)
		{
			case C_ARM_R:
			case C_ARM_L:
				return pos[spine_arm_branch];
			case C_LEG_L:
			case C_LEG_R:
				return pos[spine_leg_branch];
			default:
				return pos[spine_leg_branch];
		}
	}
}

