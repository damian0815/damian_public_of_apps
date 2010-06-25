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

	void draw();
	
	void setTarget( int which_leaf, ofxVec3f target );
	ofxVec3f getTarget( int which_leaf ) { return leaf_targets.at(which_leaf); }

private:
	
	vector<int> leaf_bones;
	vector<ofxVec3f> leaf_targets;
	
	CalSkeleton* skeleton;
	
	
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
