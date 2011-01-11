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

class ofSynthUnit;

#include "ofMain.h"

/** ofSynthMixer
 
 The Mixer that underlies the ofSynth system.
 
 Responsible for coordinating synth rendering.
 
 */

class ofSynthMixer
{
public:	
	ofSynthMixer();
	~ofSynthMixer();
	
	static ofSynthMixer* getInstance();
	
	/// Connect these units together.
	void addConnection( ofSynthUnit* from, ofSynthUnit* to );
	/// Add input as an input node to the mixer.
	void addMixerInput( ofSynthUnit* input );
	/// Remove the given unit from the DSP graph and sever all 
	/// its connections.
	void removeUnit( ofSynthUnit* unit );
	
	
	/// Set the volume of the mixer input for the given unit to vol.
	void setVolume( ofSynthUnit* input, float vol );
	
	
	/// Render the DSP chain. output is interleaved and has space for 
	/// numFrames*numChannels floats.
	void render( float* output, int numFrames, int numChannels );
	
	
	/// Lock the DSP chain.
	void lock() { mutex.lock(); }
	/// Unlock the DSP chain.
	void unlock() { mutex.unlock(); }
	
private:
	
	ofMutex mutex;

	
	// the DSP graph is composed of Edges
	struct Edge 
	{ 
		ofSynthUnit* from; 
		ofSynthUnit* to; 
		Edge( ofSynthUnit* f, ofSynthUnit* t ) 
		{ 
			from = f; to = t; 
		} 
	};
	vector<Edge> graph;

	// Return true if adding this edge to the graph would create a cycle
	bool newEdgeWouldCreateCycle( const Edge& test_edge );
	// Return true if adding this edge to the graph would create a branch
	bool newEdgeWouldCreateBranch( const Edge& test_edge );
	
	// Return all the units that are immediate (1 edge) downstream neighbours of the given unit
	vector<ofSynthUnit*> getImmediateDownstreamNeighbourUnits( ofSynthUnit* unit );

	
	// Render the linear chain portion of the DSP graph ending at tail to 
	// output. 'Linear' means no branches and no cycles.
	void renderSimpleChainWithTail( ofSynthUnit* tail, float * output, int numFrames, int numChannels );
	
	
	
	
	// Inputs into the mixer, with volume and pan
	struct MixerInput
	{
		ofSynthUnit* unit;
		float volume;
		float pan;
		MixerInput( ofSynthUnit* u, float v, float p )
		{
			unit = u; volume = v; pan = p;
		}
	};
	vector<MixerInput> inputs;
	
	
	/*
	// compiled DSP calls in render-order
	struct UnitCall
	{
		ofSynthUnit* unit;
		float* inputBuffer;
		float* outputBuffer;
		UnitCall( ofSynthUnit* u, float* i, float* o )
		{
			unit = u; inputBuffer = i; outputBuffer = o;
		}
	};
	struct UnitCallTail
	{
		ofSynthUnit* unit;
		float* outputBuffer;
		UnitCallTail( ofSynthUnit* u, float* o )
		{
			unit = u; outputBuffer = o;
		}
	};
	vector<UnitCall> calls;
	
	
	// compile the DSP chain. work backwards, starting at tails. cannot handle branches.
	void compileDSPChainWithTails( deque<UnitCallTail> tails );
	*/
	
};

