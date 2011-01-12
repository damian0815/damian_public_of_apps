/*
 *  ofSynthEffectVolume.h
 *  ofSynth
 *
 *  Created by damian on 11/01/11.
 *  Copyright 2011 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "ofSynthUnit.h"
#include "ofSynthDeclickedFloat.h"

class ofSynthEffectVolume : public ofSynthEffect
{
public:	
	ofSynthEffectVolume() { setVolume( 1.0f ); }
	
	/// Return our name
	string getName() { return "ofSynthEffectVolume"; }
	/// We are stereo
	bool isMono() { return false; }
	
	/// Set our volume to vol
	void setVolume( float vol ) { volume = vol; }
	/// Adjust our current volume by adjustment
	void adjustVolume( float adjustment ) { volume += adjustment; }
	/// Get current volume (not declicked)
	float getVolume() { return volume.getRawValue(); }
	
	void process( float* input, float* output, int numFrames, int numInChannels, int numOutChannels );
	
private:
	ofSynthDeclickedFloat volume;
	
	
};
