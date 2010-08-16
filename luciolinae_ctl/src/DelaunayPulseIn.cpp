/*
 *  DelaunayPulseIn.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 20/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "DelaunayPulseIn.h"
#include "ofxVectorMath.h"
#include "Lights.h"
#include "LightsDelaunay.h"
#include "Osc.h"

void DelaunayPulseIn::start ( int which_target, float start_radius, float _max_brightness, float _speed )
{
	target_index = which_target;
	speed = _speed;
	initial_radius = start_radius;
	max_brightness = _max_brightness;
	while ( !queued_pulses.empty() )
		queued_pulses.pop();
	// find all lights closer than start_radius and add
	ofxVec2f source_pos( lights->getLight(which_target).getX(), lights->getLight(which_target).getY() );
	const vector< int >& valid_lights = delaunay->getLights();
	for ( int i=0; i< valid_lights.size(); i++ )
	{
		ofxVec2f test_pos( lights->getLight(valid_lights[i]).getX(), lights->getLight(valid_lights[i]).getY() );
		float distance = (source_pos-test_pos).length();
		if ( distance < start_radius  && distance > start_radius*0.7f )
		{
			float brightness_pct = 1.0f-((distance-start_radius*0.7f)/(start_radius*0.7f));
			float brightness = max_brightness*brightness_pct/**brightness_pct*/;
			brightness *= ofRandom( 0.8f, 1.2f );
			float rand = ofRandomuf();
			rand*=rand;
			queued_pulses.push( MovingPulse( valid_lights[i], brightness, rand*0.2f ) );
		}
	}
	timer = 0;
	
}

void DelaunayPulseIn::update( float elapsed )
{
	timer += elapsed;
	// update animation
	//printf("queue contains: ");
	vector<MovingPulse> dequeued;
	while ( !queued_pulses.empty() && queued_pulses.top().timer < timer )
	{
		// dequeue
		dequeued.push_back( queued_pulses.top() );
		
		// remove from queue
		queued_pulses.pop();
	}

	// pings
	vector<pair< int, float > > pings;

	// coalesce pulses for the same node
	map<int,int> coalesced;
	for ( int i=0; i<dequeued.size(); i++ )
	{
		int id = dequeued[i].id;
		// add ping here (because we might drop it later)
		pings.push_back( make_pair( id, dequeued[i].energy ) );
		// coalesce
		if ( coalesced.find( id ) == coalesced.end() )
		{
			coalesced[id] = i;
		}
		else
		{
			// merge
			dequeued[coalesced[id]].energy += dequeued[i].energy;
			// remove
			dequeued.erase( dequeued.begin()+i );
			i--;
		}
	}
	// no go through coalesced
	for ( int i=0; i<dequeued.size(); i++ )
	{
		// fetch id
		int curr = dequeued[i].id;
		float energy = dequeued[i].energy;
		//printf("%2i:%7.5f ", curr, energy );

		// pulse the led
		if ( dequeued[i].timer > 0 )
			lights->pulse( curr, energy );
		// finished?
		if ( curr == target_index || energy < 0.001f )
			continue;
		
		// add the closest neighbours to queue
		set<int> adj = delaunay->getNeighbours( curr );
		ofxVec2f curr_pos( lights->getLight( curr ).getX(), 
						  lights->getLight( curr ).getY() );
		ofxVec2f target_pos( lights->getLight( target_index ).getX(),
							lights->getLight( target_index ).getY() );
		// use for calculating dot product (-> angle )
		ofxVec2f target_delta = target_pos - curr_pos;
		float distance_to_target = target_delta.length();
		target_delta /= distance_to_target;
		//printf("delta to target is %7.5f %7.5f\n", target_delta.x, target_delta.y );
		for ( set<int>::iterator jt = adj.begin();
			 jt != adj.end(); 
			 ++jt )
		{
			// add neighbours, distributing energy
			ofxVec2f adj_pos( lights->getLight( *jt ).getX(), 
							 lights->getLight( *jt ).getY() );
			ofxVec2f adj_delta = adj_pos - curr_pos;
			float distance = adj_delta.length();
			adj_delta /= distance;
			
			// dot product
			float direction_accuracy = target_delta.dot( adj_delta );
			// would this path take us towards the target?
			//printf(" adj delta to next is %7.5f %f7.5 -> accuracy %7.5f\n", adj_delta.x, adj_delta.y, direction_accuracy );
			if ( direction_accuracy > 0.5f )
			{
				direction_accuracy*=direction_accuracy;
				// yes
				//float new_energy = direction_accuracy*0.5f*(max_brightness*(1.0f-(distance_to_target/initial_radius)));
				float falloff = 0.25f;
				float new_energy = energy*direction_accuracy*0.5f - energy*distance*falloff;
				queued_pulses.push( MovingPulse(*jt, new_energy, ofRandom(0.8f,1.2f)*(timer+(distance/speed)) ) );
			}
		}
	}
	//printf("\n");
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
