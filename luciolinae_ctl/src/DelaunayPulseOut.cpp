/*
 *  DelaunayPulseOut.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 20/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "DelaunayPulseOut.h"

#include "Lights.h"
#include "LightsDelaunay.h"
#include "ofxVectorMath.h"
#include "Osc.h"


void DelaunayPulseOut::start( int which_source, float brightness, float _falloff, float _speed )
{
	speed = _speed;
	falloff = _falloff;
	seen.clear();
	while ( !queued_pulses.empty() )
		queued_pulses.pop();
	queued_pulses.push( MovingPulse( which_source, brightness, ofRandom(0.0f,0.2f) ) );
	timer = 0;
}

void DelaunayPulseOut::update( float elapsed )
{
	timer += elapsed;
	vector<pair< int, float > > pings;
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
			lights->pulse( curr, energy );
			
			// queue a ping
			pings.push_back( make_pair( curr, energy ) );
			
			// add to seen
			seen.insert( curr );
			
			// add neighbours to queue
			set<int> adj = delaunay->getNeighbours( curr );
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
					queued_pulses.push( MovingPulse(*jt, energy*max(0.0f,1-falloff*distance), ofRandom(0.8f,1.2f)*(timer+(distance/speed)) ) );
				}
			}
		}
	}
	// send pings
	for ( int i=0; i<pings.size(); i++ )
	{
		ofxOscMessage m;
		m.setAddress( "/delaunay/ping" );
		m.addFloatArg( pings[i].second );
		const Light& light = lights->getLight( pings[i].first );
		m.addFloatArg( light.getX() );
		m.addFloatArg( light.getY() );
		Osc::getInstance()->sendMessage( m );
	}
}

