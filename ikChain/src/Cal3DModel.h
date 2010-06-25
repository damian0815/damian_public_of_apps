/*
 *  Cal3DModel.h
 *  ikChain
 *
 *  Created by damian on 22/06/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "cal3d.h"
using namespace std;

class Cal3DModel
{
public:
	Cal3DModel();
	~Cal3DModel();
	
	bool setup( string name, string skeleton_file, string mesh_file );

	void draw( float scale = 1.0f );
	
	/// update the mesh based on the current skeleton state
	void updateMesh( );
	
	void dumpSkeleton();
	
	/// access to the skeleton
	/// if updating bone position/orientation, call updateMesh to push changes
	/// to the mesh.
	CalSkeleton* getSkeleton() { return instance->getSkeleton(); }
	
	void rotateBoneX( int bone_id, float amount );
	
private:
	
	int num_bones;
	
	CalCoreModel* model;
	CalModel* instance;

	void dumpSkeletonImp( CalCoreBone* root, string prefix );
	
};
