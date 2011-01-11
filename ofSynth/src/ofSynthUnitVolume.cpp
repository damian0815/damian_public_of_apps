/*
 *  ofSynthUnitVolume.cpp
 *  ofSynth
 *
 *  Created by damian on 11/01/11.
 *  Copyright 2011 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include <string>
using namespace std;
#include "ofSynthUnitVolume.h"



void ofSynthUnitVolume::process( float* input, float* output, int numFrames, int numChannels )
{
	volume.rebuildRampIfNecessary();
	
	// loop through all the frames
	for ( int i=0; i<numFrames; i++ ) 
	{
		// write value to all the channels
		for ( int j=0; j<numChannels; j++ )
		{
			output[i*numChannels+j] = input[i*numChannels+j] * volume.getDeclickedValue(); 
		}
		volume.frameTick();
	}
}
