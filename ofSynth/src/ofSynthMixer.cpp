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

ofSynthMixer* ofSynthMixer::getInstance()
{
	if ( instance == NULL )
		instance = new ofSynthMixer();
	
	return instance;
}




void ofSynthMixer::addMixerInput( ofSynthUnit* input )
{
	mutex.lock();
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
	
	mutex.unlock();
	
}



void ofSynthMixer::removeUnit( ofSynthUnit* unit )
{
	mutex.lock();

	// remove all edges from graph
	for ( int i=0; i<graph.size(); i++ )
	{
		Edge& e = graph[i];
		if ( e.from == unit || e.to == unit )
		{
			// remove from the graph
			graph.erase( graph.begin()+i );
			--i;
		}
	}
	
	// remove from inputs list if necessary
	for ( int i=0; i<inputs.size() ;i++ )
	{
		MixerInput& input = inputs[i];
		if ( input.unit == unit )
		{
			inputs.erase( inputs.begin()+i );
			i--;
		}
	}
	
	mutex.unlock();
}


void ofSynthMixer::addConnection( ofSynthUnit* from, ofSynthUnit* to )
{
	mutex.lock();

	Edge e = Edge( from, to );
	if ( newEdgeWouldCreateCycle( e ) )
	{
		ofLog( OF_LOG_ERROR, "ofSynthMixer: can't connect '%s' (%x) to '%s' (%x): this would create a cycle in the DSP graph",
			  from->getName().c_str(), from, to->getName().c_str(), to );
		return;
	}
	
	if ( newEdgeWouldCreateBranch( e ) )
	{
		ofLog( OF_LOG_ERROR, "ofSynthMixer: can't connect '%s' (%x) to '%s' (%x): this would create a branch in the DSP graph",
			  from->getName().c_str(), from, to->getName().c_str(), to );
		return;
	}
	
	graph.push_back( e );
	
	mutex.unlock();

}





// Return true if adding this edge to the graph would create a cycle
bool ofSynthMixer::newEdgeWouldCreateCycle( const Edge& test_edge )
{

	// assuming the graph has no cycles from the start, can we trace a path 
	// from test_edge.to to test_edge.from? if we can, then adding test_edge will create a cycle

	// do a depth-first traversal
	deque<ofSynthUnit*> stack;
	stack.push_back( test_edge.to );
	while ( !stack.empty() )
	{
		ofSynthUnit* u = stack.back();
		stack.pop_back();
		// if u is test_edge.from, then we have looped round to the beginning
		if ( u == test_edge.from )
			return true;

		// fetch all immediate downstream neighbours
		vector<ofSynthUnit*> downstream = getImmediateDownstreamNeighbourUnits( u );
		// copy downstream to the back of stack
		copy( downstream.begin(), downstream.end(), back_inserter( stack ) );
	}
	
	// if we made it here, the cycle test has failed to find a cycle
	return false;
}


// Return true if adding this edge to the graph would create a branch
bool ofSynthMixer::newEdgeWouldCreateBranch( const Edge& test_edge )
{
	set<ofSynthUnit*> inputs;
	set<ofSynthUnit*> outputs;
	
	// loop through all the edges on the graph. if a unit is used as an input or
	// an output more than once, then we have a branch.
	for ( int i=0; i<graph.size(); i++ )
	{
		inputs.insert( graph[i].from );
		outputs.insert( graph[i].to );
	}
	
	// check if the units on the new edge are already in use as inputs or outputs
	if ( inputs.find( test_edge.from ) != inputs.end() )
		return true;
	if ( outputs.find( test_edge.to ) != outputs.end() )
		return true;

	// all good
	return false;
}



// Return all the units that are immediate (1 edge) downstream neighbours of the given unit
vector<ofSynthUnit*> ofSynthMixer::getImmediateDownstreamNeighbourUnits( ofSynthUnit* unit )
{
	vector<ofSynthUnit*> result;
	for ( int i=0; i<graph.size(); i++ )
	{
		if ( graph[i].from == unit )
			result.push_back( graph[i].to );
	}
	return result;
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

	mutex.lock();

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
	
	mutex.unlock();
	
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





