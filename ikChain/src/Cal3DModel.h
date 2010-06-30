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
	
	/// setup a new CalCoreModel with the given name, skeleton file and mesh file
	bool setup( string name, string skeleton_file, string mesh_file );
	
	/// load the given animation; return the name of the loaded anim in anim_name
	/// return true on success, false on failure
	bool loadAnimation( const string& anim_file, string& anim_name );
	
	/// instantiate the loaded model
	bool createInstance();

	/// update the mesh based on the current skeleton state
	void updateMesh( );
	/// reset skeleton to rest position
	void resetToRest();
	
	/// access to the skeleton
	/// if updating bone position/orientation, call updateMesh to push changes
	/// to the mesh.
	CalSkeleton* getSkeleton() { return instance->getSkeleton(); }

	
	/// start playing the first animation
	void playFirstAnimation();
	

	/// draw the model
	void draw( float scale = 1.0f );
	/// draw just the bones of the model (for debugging)
	void drawBones( float scale = 1.0f );
	
	
	
	/// dump the loaded skeleton structure to the console
	void dumpSkeleton();
	
	/// rotate the given bone about x by the given angle
	void rotateBoneX( int id, float amount );

	
private:
	
	int num_bones;
	
	CalCoreModel* model;
	CalModel* instance;
	int mesh_id;

	void dumpSkeletonImp( CalCoreBone* root, string prefix );
	void drawBones(  int bone_id, float scale );
	
};
