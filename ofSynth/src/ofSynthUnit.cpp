/*
 *  ofSynthUnit.cpp
 *  ofSynth
 *
 *  Created by damian on 10/01/11.
 *  Copyright 2011 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "ofSynthUnit.h"
#include "ofSynthMixer.h"

vector<ofSynthGenerator*> ofSynthUnit::getInputs()
{
	// no inputs
	return vector<ofSynthGenerator*>();
}



void ofSynthReceiver::setSampleRate( int rate )
{
	ofLog(OF_LOG_WARNING, "ofSynthReceiver::setSampleRate %i", rate );

	vector<ofSynthGenerator*> inputs = getInputs();
	for ( int i=0; i<inputs.size(); i++ ) 
	{ 
		inputs[i]->setSampleRate( rate ); 
	}
	sampleRate = rate;
}

bool ofSynthReceiver::addInputFrom( ofSynthGenerator* from ) 
{
	// check for existing input
	if ( input != NULL )
	{
		ofLog( OF_LOG_ERROR, "ofSynthUnit: can't connect '%s' (%x) to '%s' (%x): it already has an input",
			  from->getName().c_str(), from, this->getName().c_str(), this );
		return false;
	}
	// check for branches/cycles
	if ( addingInputWouldCreateCycle( from ) )
	{
		ofLog( OF_LOG_ERROR, "ofSynthUnit: can't connect '%s' (%x) to '%s' (%x): this would create a cycle in the DSP graph",
			  from->getName().c_str(), from, this->getName().c_str(), this );
		return false;
	}

	input = from;
	from->setSampleRate( sampleRate );
	return true;
}

vector<ofSynthGenerator*> ofSynthReceiver::getInputs()
{
	vector<ofSynthGenerator*> result;
	if ( input )
	{
		result.push_back( input );
	}
	return result;
}





// Return true if adding this edge to the graph would create a cycle
bool ofSynthReceiver::addingInputWouldCreateCycle( ofSynthGenerator* test_input )
{
	
	// assuming the graph has no cycles from the start, can we already trace a path 
	// from test_input to ourselves? if we can, then adding test_input will create 
	// a cycle.
	
	// do a depth-first traversal
	deque<ofSynthUnit*> stack;
	stack.push_back( this );
	while ( !stack.empty() )
	{
		ofSynthUnit* u = stack.back();
		stack.pop_back();
		// if u is test_edge.from, then we have looped round to the beginning
		if ( u == test_input )
		{
			return true;
		}
		
		// fetch all immediate upstream neighbours
		vector<ofSynthGenerator*> upstream = u->getInputs();
		// copy downstream to the back of stack
		copy( upstream.begin(), upstream.end(), back_inserter( stack ) );
	}
	
	// if we made it here, the cycle test has failed to find a cycle
	return false;
}




void ofSynthEffect::audioRequested( float* buffer, int numFrames, int numChannels )
{
	// fetch data from input, and render via process() function
	// create/recreate input buffer if necessary
	if ( inputBuffer.numFrames != numFrames || inputBuffer.numChannels != numChannels )
	{
		inputBuffer.allocate( numFrames, numChannels );
	}
	
	// call input's generate
	if ( !input )
	{
		inputBuffer.clear();
	}
	else
	{
		input->audioRequested( inputBuffer.buffer, inputBuffer.numFrames, inputBuffer.numChannels );
	}
	
	// call process
	process( inputBuffer.buffer, buffer, numFrames, inputBuffer.numChannels, numChannels );
}


void ofSynthEffect::setSampleRate( int rate )
{
	ofSynthGenerator::setSampleRate( rate );
	ofSynthReceiver::setSampleRate( rate );
}



void ofSynthEffectPassthrough::process( float* input, float *output, int numFrames, int numInChannels, int numOutChannels )
{
	// passthrough
	
	memcpy( output, input, numFrames*numOutChannels*sizeof(float) );
}

