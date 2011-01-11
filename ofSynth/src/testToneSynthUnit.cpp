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



void testToneSynthUnit::setFrequency( float frequency )
{
	// calculate a phase advance per audio frame (sample)
	// basically, every OFSYNTH_SAMPLE_RATE frames (1s of audio), we want
	// to advance phase by frequency*TWO_PI.
	phaseAdvancePerFrame = (1.0f/OFSYNTH_SAMPLE_RATE)*frequency*TWO_PI;
}

void testToneSynthUnit::process( float* input, float* output, int numFrames, int numChannels )
{
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
		phase += phaseAdvancePerFrame;
	}
	// wrap phase to 0..TWO_PI
	phase = remainderf( phase, TWO_PI );
	
	
}
