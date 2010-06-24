/*
 *  Cal3DModel.cpp
 *  ikChain
 *
 *  Created by damian on 22/06/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "Cal3DModel.h"
#include "ofMain.h"

Cal3DModel::Cal3DModel( )
: model(0), instance(0)
{
}


bool Cal3DModel::setup( string name, string skeleton_file )
{
	model = new CalCoreModel( name );
	
	if ( !model->loadCoreSkeleton( ofToDataPath(skeleton_file) ) )
	{
		printf("couldn't load skeleton %s\n", skeleton_file.c_str() );
		return false;
	}

	instance = new CalModel( model );
	
	return true;
}

void Cal3DModel::dumpSkeleton()
{
	// walk through skeleton
	printf("dumping bones:\n");
	CalSkeleton* skeleton = instance->getSkeleton();
	vector<int> core_root_bones = skeleton->getCoreSkeleton()->getVectorRootCoreBoneId();
	for ( int i=0; i<core_root_bones.size(); i++ )
	{
		printf("core %i %i %s\n", i, core_root_bones[i], skeleton->getBone( core_root_bones[i] )->getCoreBone()->getName().c_str() );
		
		dumpSkeletonImp( skeleton->getBone( core_root_bones[i] )->getCoreBone(), " " );
	}
}

void Cal3DModel::dumpSkeletonImp( CalCoreBone* b, string prefix )
{
	printf("%s - %s\n", prefix.c_str(), b->getName().c_str() );
	list<int> children = b->getListChildId();
	for ( list<int>::iterator ch = children.begin(); ch != children.end(); ++ch )
	{
		dumpSkeletonImp( b->getCoreSkeleton()->getCoreBone( *ch ), prefix+"  " );
	}
}

void Cal3DModel::draw()
{
	// get the renderer of the model
	CalRenderer *pCalRenderer;
	pCalRenderer = instance->getRenderer();
	
	// begin the rendering loop
	if(!pCalRenderer->beginRendering())
	{
		// error handling ...
	}
	
	// get the number of meshes
	int meshCount;
	meshCount = pCalRenderer->getMeshCount();
	
	
	pCalRenderer->endRendering();
}



Cal3DModel::~Cal3DModel() { 
	if ( instance )
		delete instance;
}