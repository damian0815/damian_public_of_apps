/*
 *  ofSynthMixer.h
 *  ofSynth
 *
 *  Created by damian on 10/01/11.
 *  Copyright 2011 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include <vector>
using namespace std;
#include <deque>

#include "ofMain.h"

#include "ofSynthUnit.h"

/** ofSynthMixer
 
 The Mixer that underlies the ofSynth system.
 
 Responsible for coordinating synth rendering.
 
 */

class ofSynthMixer : public ofSynthReceiver, public ofSynthGenerator
{
public:	
	ofSynthMixer();
	~ofSynthMixer();

	string getName() { return "ofSynthMixer"; }
	
	/// Set the volume of the mixer input for the given unit to vol.
	void setVolume( ofSynthGenerator* input, float vol );
	
	/// Add an input to this unit from the given source unit.
	bool addInputFrom( ofSynthGenerator* source );
	
	/// render
	void audioRequested( float* buffer, int numFrames, int numChannels );

	/// return all our inputs
	vector<ofSynthGenerator*> getInputs();

	/// pass sample rate changes on to inputs
	void setSampleRate( int rate );
	
private:
	
	/// Lock the DSP chain.
	void lock() { mutex.lock(); }
	/// Unlock the DSP chain.
	void unlock() { mutex.unlock(); }
	
	ofMutex mutex;

	
	// Inputs into the mixer, with volume and pan
	struct MixerInput
	{
		ofSynthGenerator* input;
		ofSoundBuffer inputBuffer;
		float volume;
		float pan;
		MixerInput( ofSynthGenerator* i, float v, float p )
		{
			input = i;
			volume = v; 
			pan = p;
		}
	};
	vector<MixerInput> inputs;
	
};

