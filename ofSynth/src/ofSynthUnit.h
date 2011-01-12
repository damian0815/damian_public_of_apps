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

#include "ofSoundStream.h"
#include <vector>
using namespace std;


class ofSoundBuffer
{
public:
	
	float* buffer;
	int numFrames;
	int numChannels;
	
	ofSoundBuffer() { numFrames = 0; numChannels = 0; buffer = NULL; }
	ofSoundBuffer( int nFrames, int nChannels ) { numFrames = nFrames; numChannels = nChannels; buffer = new float[nFrames*nChannels]; }
	~ofSoundBuffer() { if ( buffer ) delete[] buffer; }
	
	void clear() { if ( buffer ) memset( buffer, 0, sizeof(float)*numFrames*numChannels); }
	void allocate( int nFrames, int nChannels ) 
	{ 
		if ( !buffer || numFrames != nFrames || numChannels != nChannels )
		{
			numFrames = nFrames; numChannels = nChannels; 
			if ( buffer )
			{
				delete[] buffer;
			}
			
			buffer = new float[nFrames*nChannels];
		}
	}
	void copyChannel( int channel, float* output ) const
	{
		if ( buffer && channel < numChannels )
		{
			for ( int i=0; i<numFrames; i++ )
			{
				output[i] = buffer[numChannels*i + channel];
			}
		}
	}
};



class ofSynthGenerator;

class ofSynthUnit
{
public:	
	
	/// Return the name of this synth unit.
	virtual string getName() = 0;

	/// Return our inputs in a vector (but at ofSynthUnit level we have no inputs).
	virtual vector<ofSynthGenerator*> getInputs();
	
protected:
	
};



class ofSynthGenerator: public ofSynthUnit, public ofSoundSource
{
public:

	virtual void audioRequested( float* buffer, int numFrames, int numChannels ) = 0;
	
protected:
};


class ofSynthReceiver: public ofSynthUnit
{
public:
	ofSynthReceiver() { input = NULL; }
	
	/// Set the sample rate of this synth unit. If you overload this remember to call the base class.
	virtual void setSampleRate( int rate );

	/// Add an input to this unit from the given source unit. If overloading remember to call base.
	virtual bool addInputFrom( ofSynthGenerator* source );
	
	/// Return our inputs in a vector.
	virtual vector<ofSynthGenerator*> getInputs();
	
protected:
	// walk the DSP graph and see if adding test_input as an input to ourselves; return true if it would
	bool addingInputWouldCreateCycle( ofSynthGenerator* test_input );

	
	
/*	struct SynthReceiverInput
	{
		ofSynthGenerator* unit;
		ofSoundBuffer inputBuffer;
		SynthReceiverInput() { unit = NULL; }
		SynthReceiverInput( ofSynthGenerator* i ) { unit = i; }
	};
	vector< SynthReceiverInput > inputs;*/
	ofSynthGenerator* input;
	ofSoundBuffer inputBuffer;

	float sampleRate;
};


class ofSynthEffect: public ofSynthGenerator, public ofSynthReceiver
{
public:
	
	/// Pass sample rate changes through; if your override this remember to call base.
	void setSampleRate( int rate );
	
	/// Override this function to do your own processing. Be sure to match numInChannels and numOutChannels
	virtual void process( float* input, float *output, int numFrames, int numInChannels, int numOutChannels ) = 0;
	

	/// implementation of the generate function from ofSynthGenerator
	void audioRequested( float* buffer, int numFrames, int numChannels );
};


class ofSynthEffectPassthrough: public ofSynthEffect
{
public:
	string getName() { return "ofSynthEffectPassthrough"; }
	
	virtual void process( float* input, float *output, int numFrames, int numInChannels, int numOutChannels );
	
	/// return the buffer we saw as we passed it through
	ofSoundBuffer& getBuffer() { return inputBuffer; }
};
