/*
 *  ofSynthDeclickedFloat.h
 *  ofSynth
 *
 *  Created by damian on 11/01/11.
 *  Copyright 2011 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

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

