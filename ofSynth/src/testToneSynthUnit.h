/*
 *  testToneSynthUnit.h
 *  ofSynth
 *
 *  Created by damian on 11/01/11.
 *  Copyright 2011 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "ofSynthUnit.h"


class testToneSynthUnit: public ofSynthUnit
{
public:
	
	testToneSynthUnit() { phase = 0; }

	/// Return our name
	string getName() { return "testToneSynthUnit"; }
	
	/// We are stereo
	bool isMono() { return false; }
	
	void process( float* input, float* output, int numFrames, int numChannels );
	
	
private:
	
	float phase;
	
};


