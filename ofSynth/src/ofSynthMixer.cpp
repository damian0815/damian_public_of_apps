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

ofSynthMixer* instance = NULL;

ofSynthMixer::ofSynthMixer()
{
	instance = this;
}

ofSynthMixer::~ofSynthMixer()
{
	instance = NULL;
}

ofSynthMixer* ofSynthMixer::getInstance()
{
	if ( instance == NULL )
		instance = new ofSynthMixer();
	
	return instance;
}




void ofSynthMixer::addMixerInput( ofSynthUnit* input )
{
	// check for existing
	for ( int i=0; i<inputs.size(); i++ )
	{
		if ( inputs[i].unit == input )
		{
			ofLog( OF_LOG_ERROR, "Can't add mixer input for ofSynthUnit %s (%x): mixer input already exists", input->getName().c_str(), input );
			return;
		}
	}
	
	// add with default volume (1.0f) and pan (0.5f)
	inputs.push_back( MixerInput( input, 1.0f, 0.5f ) );
	
}



void ofSynthMixer::removeUnit( ofSynthUnit* input )
{
	// remove all edges from graph
	
	// remove all inputs
}


void ofSynthMixer::setVolume( ofSynthUnit* input, float vol )
{
	for ( int i=0; i<inputs.size(); i++ )
	{
		if ( inputs[i].unit == input )
		{
			inputs[i].volume = vol;
		}
	}
}


// Render the DSP chain. output is interleaved and has space for 
// numFrames*numChannels floats.
void ofSynthMixer::render( float* output, int numFrames, int numChannels )
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

//	deque<UnitCallTail> tails;
	// allocate working space
	float working[numFrames*numChannels*sizeof(float)];
	for ( int i=0; i<inputs.size(); i++ )
	{
		// clear working
		memset( working, 0, numFrames*numChannels*sizeof(float) );
		if ( inputs[i].unit->isMono() != isMono )
		{
			ofLog( OF_LOG_ERROR, "Mixer: can't process input %i: mono/stereo up/downmix not implemented yet", i );
		}
		else
		{
			// render input into working
			renderSimpleChainWithTail( inputs[i].unit, working, numFrames, numChannels );
			/*
			float* working = gimmeNewBuffer( numFrames*numChannels );
			tails.push_back( UnitCallTail( inputs[i].unit, working ) );
			 */
		}
		
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
	
	
//	compileDSPChainWithTails( tails );
	
}

void ofSynthMixer::renderSimpleChainWithTail( ofSynthUnit* tail, float * output, int numFrames, int numChannels )
{
	// walk backwards to the front of the chain
	for ( int i=0; i<graph.size(); i++ )
	{
		if ( graph[i].to == tail )
		{
			// recursively render ancestors
			renderSimpleChainWithTail( graph[i].from, output, numFrames, numChannels );
			break;
		}
	}
	// check number of channels
	bool isMono = ( numChannels == 1 );
	if ( isMono == tail->isMono() )
		// render in-place
		tail->process( output, output, numFrames, numChannels );
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





