/*
 *  ofSynthMixer.cpp
 *  ofSynth
 *
 *  Created by damian on 10/01/11.
 *  Copyright 2011 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "ofSynthMixer.h"
#include "ofSynthUnit.h"
#include "ofMain.h"
#include <set>

ofSynthMixer* instance = NULL;

ofSynthMixer::ofSynthMixer()
{
	instance = this;
}

ofSynthMixer::~ofSynthMixer()
{
	instance = NULL;
}



void ofSynthMixer::setVolume( ofSynthGenerator* input, float vol )
{
	for ( int i=0; i<inputs.size(); i++ )
	{
		if ( inputs[i].input == input )
		{
			inputs[i].volume = vol;
		}
	}
}


// Render the DSP chain. output is interleaved and has space for 
// numFrames*numChannels floats.
void ofSynthMixer::audioRequested( float* output, int numFrames, int numChannels )
{
	// advance DSP
	if ( numChannels != 1 && numChannels != 2 )
	{
		ofLog( OF_LOG_ERROR, "only 1 or 2 channels supported");
		return;
	}
	bool isMono = ( numChannels == 1 );
	
	// write to output
	// clear output array
	memset( output, 0, numFrames*numChannels*sizeof(float) );

	mutex.lock();

	// allocate working space
	float working[numFrames*numChannels];
	for ( int i=0; i<inputs.size(); i++ )
	{
		// clear working
		memset( working, 0, numFrames*numChannels*sizeof(float) );

		// render input into working
		inputs[i].input->audioRequested( working, numFrames, numChannels );
		
		// construct precalculated volume + pan array (for efficiency)
		float volumePerChannel[numChannels];
		float vol_l = inputs[i].volume*(1.0f-inputs[i].pan);
		float vol_r = inputs[i].volume*inputs[i].pan;
		for ( int j=0; j<numChannels; j++ )
		{
			volumePerChannel[j] = (j==0?vol_l:vol_r);
		}
		// mix working into output, respecting pan and volume and preserving interleaving
		for ( int j=0; j<numFrames*numChannels; j++ )
		{
			output[j] += working[j]*volumePerChannel[i%numChannels];
		}
	}
	
	mutex.unlock();
	
//	compileDSPChainWithTails( tails );
	
}

/*
void ofSynthMixer::compileDSPChainWithTails( deque<UnitCallTail> tails )
{
	// for everything in tails
	
	while ( !tails.empty() )
	{
		// get all the audio sources that contribute to this unit
		ofSynthUnit* unit = tails.back().unit;
		vector<ofSynthUnit*> sources = getAudioSourcesFor( unit );
		
		
		tails.pop_back();
	}
	
	
}*/



/// Add an input to this unit from the given source unit
bool ofSynthMixer::addInputFrom( ofSynthGenerator* source )
{
	// check for branches/cycles
	if ( addingInputWouldCreateCycle( source ) )
	{
		ofLog( OF_LOG_ERROR, "ofSynthUnit: can't connect '%s' (%x) to '%s' (%x): this would create a cycle in the DSP graph",
			  source->getName().c_str(), source, this->getName().c_str(), this );
		return false;
	}
	
	inputs.push_back( MixerInput( source, 1.0f, 0.5f ) );
	source->setSampleRate( sampleRate );
}


vector<ofSynthGenerator*> ofSynthMixer::getInputs()
{
	vector<ofSynthGenerator*> result;
	for ( int i =0; i<inputs.size(); i++ )
	{
		result.push_back( inputs[i].input );
	}
	return result;
}


void ofSynthMixer::setSampleRate( int rate )
{
	ofLog(OF_LOG_WARNING, "ofSynthMixer::setSampleRate(%i)", rate );
	for ( int i =0; i<inputs.size(); i++ )
	{
		ofLog(OF_LOG_WARNING, "ofSynthMixer::setSampleRate -> input %x", inputs[i].input );
		inputs[i].input->setSampleRate( rate );
	}
	ofSynthGenerator::setSampleRate( rate );
	ofSynthReceiver::setSampleRate( rate );
}


