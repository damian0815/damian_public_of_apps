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
	
	bool setup( string name, string skeleton_file );

	void draw();
	
	void dumpSkeleton();
	
	CalCoreSkeleton* getCoreSkeleton() { return instance->getSkeleton()->getCoreSkeleton(); }
	
private:
	
	CalCoreModel* model;
	CalModel* instance;

	void dumpSkeletonImp( CalCoreBone* root, string prefix );
	
};
