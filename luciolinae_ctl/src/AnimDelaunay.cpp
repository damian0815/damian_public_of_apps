/*
 *  AnimDelaunay.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 18/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "AnimDelaunay.h"
#include "LightsDelaunay.h"
#include "ofxVec2f.h"

static const float STEP_TIME = 0.08f;

const char* AnimDelaunay::NAME = "Delaunay";


void AnimDelaunay::update( float elapsed )
{
	timer += elapsed;
	// update animation
	while ( !queued_pulses.empty() && queued_pulses.top().timer < timer )
	{
		// fetch id
		int curr = queued_pulses.top().id;
		float energy = queued_pulses.top().energy;
		// remove from queue
		queued_pulses.pop();
		// if we haven't already seen this one
		if ( seen.find( curr ) == seen.end() )
		{
			// pulse the led
			lights->pulse( curr, energy, 0 );
			
			// add to seen
			seen.insert( curr );
			
			// add neighbours to queue
			set<int> adj = lights->getDelaunay()->getNeighbours( curr );
			// check neighbours against seen. 
			ofxVec2f curr_pos( lights->getLight( curr ).getX(), 
						   lights->getLight( curr ).getY() );
			for ( set<int>::iterator jt = adj.begin();
				 jt != adj.end(); 
				 ++jt )
			{
				// add unseen neighbours 
				if ( seen.find( *jt ) == seen.end() )
				{
					ofxVec2f adj_pos( lights->getLight( *jt ).getX(), 
									  lights->getLight( *jt ).getY() );
					float distance = ( adj_pos - curr_pos ).length();
					static const float SPEED = 0.5f;
					queued_pulses.push( DelaunayPulse(*jt, energy*(1-distance), ofRandom(0.8f,1.2f)*(timer+(distance/SPEED)) ) );
				}
			}
		}
	}
}

void AnimDelaunay::keyPressed ( int k ) 
{
	if ( k == OF_KEY_RETURN )
	{
		triggerDelaunay( ofRandom( 0, lights->getNumLights()*0.999f ) );
	}
}


void AnimDelaunay::triggerDelaunay( int index )
{
	index = min(lights->getNumLights()-1,max(0,index));
	seen.clear();
	while ( !queued_pulses.empty() )
		queued_pulses.pop();
	queued_pulses.push( DelaunayPulse( index, 0.2, 0 ) );
	timer = 0;
}


void AnimDelaunay::draw()
{
	ofSetColor( 0x20, 0x80, 0x20, 0x10 );
	for ( int i=0; i<lights->getNumLights(); i++ )
	{
		set<int> adj = lights->getDelaunay()->getNeighbours( i );
		for ( set<int>::iterator jt = adj.begin();
			 jt != adj.end();
			 ++jt )
		{
			ofLine( ofGetWidth()*lights->getLight( i ).getX(),   ofGetHeight()*lights->getLight( i ).getY(),
				    ofGetWidth()*lights->getLight( *jt ).getX(), ofGetHeight()*lights->getLight( *jt ).getY() );
		}
	}
	
}
