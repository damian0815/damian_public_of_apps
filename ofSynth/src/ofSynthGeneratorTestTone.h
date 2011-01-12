/*
 *  ofSynthGeneratorTestTone.h
 *  ofSynth
 *
 *  Created by damian on 11/01/11.
 *  Copyright 2011 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "ofSynthUnit.h"

class ofSynthGeneratorTestTone: public ofSynthGenerator
{
public:	
	ofSynthGeneratorTestTone() { phase = 0; setFrequency( 440.0f ); }

	/// Return our name
	string getName() { return "ofSynthUnitTestTone"; }
	/// We are stereo
	bool isMono() { return false; }
	
	/// Set our frequency
	void setFrequency( float freq );
	/// Update generation for the new sample rate
	void setSampleRate( int rate );
	
	void audioRequested( float* output, int numFrames, int numChannels );
	
	
private:
	
	float phase;
	float frequency;
	int sampleRate;
	float phaseAdvancePerFrame;
	
};


