/*
 *  testToneSynthUnit.cpp
 *  ofSynth
 *
 *  Created by damian on 11/01/11.
 *  Copyright 2011 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "ofMain.h"
#include "testToneSynthUnit.h"

void testToneSynthUnit::process( float* input, float* output, int numFrames, int numChannels )
{
	// 440hz
	static const float PHASE_ADVANCE_PER_FRAME = (1.0f/44100.0f)*440.0f*TWO_PI;

	// loop through all the frames
	for ( int i=0; i<numFrames; i++ ) 
	{
		float value = sinf( phase );
		// write value to all the channels
		for ( int j=0; j<numChannels; j++ )
		{
			output[i*numChannels+j] = value;
		}
		// advance phase
		phase += PHASE_ADVANCE_PER_FRAME;
	}
	// wrap phase to 0..TWO_PI
	phase = remainderf( phase, TWO_PI );
	
	
}
