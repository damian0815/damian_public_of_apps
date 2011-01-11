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

static const int OFSYNTH_SAMPLE_RATE = 44100;

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


/** ofSynthDeclickedFloat

 Declick a changing float value by using a 64 sample ramp (around 1.5ms at 44.1kHz).

 You must call rebuildRampIfNecessary before processing every audio block, in order to apply incoming
 value changes.
 
 Also, you must call frameTick() for every audio frame (sample) to advance the ramp.
 
 */

class ofSynthDeclickedFloat
{
public:
	ofSynthDeclickedFloat( float startValue=0.0f ) { rampPos = 0; current = startValue; setValue( startValue ); }
	
	/// Return the current value, declicked
	float getDeclickedValue() { return current; }
	/// Return the raw value (the target for declicking)
	float getRawValue() { return target; }
	/// Set a new value + rebuild ramp
	void setValue( float newValue ) { target = newValue; rampNeedsRebuild = true; }
	
	/// Rebuild the ramp, if necessary. Call before processing a block of samples.
	void rebuildRampIfNecessary() { if ( rampNeedsRebuild ) rebuildRamp(); rampNeedsRebuild = false; }
	
	/// Update, to be called for every frame
	void frameTick() { current = ramp[rampPos]; ramp[rampPos] = target; rampPos = (rampPos+1)%64; }

	/// operator overloading
	ofSynthDeclickedFloat& operator=( float new_value ) { setValue( new_value ); return *this; }
	ofSynthDeclickedFloat& operator+=( float adjustment ) { setValue( target+adjustment ); return *this; }
	ofSynthDeclickedFloat& operator-=( float adjustment ) { setValue( target-adjustment ); return *this; }
	
private:
	
	void rebuildRamp() { float v = current; float step = (target-current)/63; for ( int i=0; i<64; i++ ) { ramp[(i+rampPos)%64] = v; v += step; } }
	
	float current;
	float target;

	bool rampNeedsRebuild;
	int rampPos;
	float ramp[64];
};




