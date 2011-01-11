/*
 *  ofSynthUnit.h
 *  ofSynth
 *
 *  Created by damian on 10/01/11.
 *  Copyright 2011 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

/** ofSynthUnit
 
 Base class for ofSynth units that can be strung together into a DSP-style chain.
 
 addOutputTo( ofSynthUnit* target ) adds an audio output from this unit to the target unit.
 
 process( float* inputBuffer, float* outputBuffer, int size ) processes the audio. size is 
 the number of (mono or stereo) frames in the input and output buffers; therefore inputBuffer/ 
 outputBuffer has space for (size*numChannels) floats, where numChannels is 2 for stereo 
 units and 1 for mono units.
 
 isMono() returns a boolean as to whether this unit does mono or stereo processing. The 
 ofSynthMixer underlying system handles mixup/mixdown from/two mono or stereo as necessary.
 
 */

#include <vector>
using namespace std;

class ofSynthUnit
{
public:	
	
	ofSynthUnit() {};
	virtual ~ofSynthUnit();
	

	/// Return the name of this synth unit.
	virtual string getName() = 0;
	
	
	/// Add an output to the target object
	void addOutputTo( ofSynthUnit* target );
	/// Add an output to the global mixer
	void addOutputToMixer();
	
	/// Process audio data. If this synth unit just does output, it will ignore inputBuffer.
	virtual void process( float* inputBuffer, float* outputBuffer, int numFrames, int numChannels ) = 0;
	
	/// Returns a boolean as to whether this unit does mono or stereo processing. 
	virtual bool isMono() = 0;
	
	
protected:
	
	
	
	vector< ofSynthUnit* > outputs;
	
};

