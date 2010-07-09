/*
 *  StateAnimSeq.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 08/06/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "StateAnimSeq.h"
#include "Util.h"
#include "Osc.h"

const char* StateAnimSeq::NAME = "state_seq";

void StateAnimSeq::enter()
{
	anim_pct = 0.0f;
	current_active = -1;
	speed = 1.0f;
	count_left = squaredRandom( 10, 50 );
	ofxOscMessage m;
	m.setAddress("/seq/start");
	Osc::sendMessage( m );
}	

void StateAnimSeq::update( float elapsed )
{
	float real_speed = speed*16*1.0f/lights->getNumLights();
	
	// update pct
	anim_pct += elapsed * real_speed;
	// calculate new active
	int new_active = anim_pct*lights->getNumLights();
	if ( new_active != current_active )
	{
		// we might jump more than one
		int dir = (real_speed>=0) ? 1:-1;
		int first, last;
		//printf("current_active %2i, new_active %2i, dir %i; ", current_active, new_active, dir );
		// have we gone full circle?
		if ( abs(new_active-current_active)>lights->getNumLights() )
		{
			// yes - then pulse everything
			last = new_active;
			first = last - dir*(lights->getNumLights()-1);
		}
		else 
		{
			last = new_active;
			first = current_active + dir;
			if ( first < 0 )
				first += lights->getNumLights();
		}
		//printf("pulsing from %i to %i\n", first, last );
		
		// pulse the new one(s)
		int count = abs(last-first)+1;
		for ( int i=0; i<count; i++ )
		{
			int which = first + dir*i;
			//printf("pulsing %i\n", which%lights->getNumLights() );
			which %= lights->getNumLights();
			ofxOscMessage m;
			m.setAddress("/seq/trigger");
			m.addIntArg( which );
			Osc::sendMessage( m );
			lights->pulse( which, 1.0f );
			speed = squaredRandom( 1.0f, 0.05f );
			count_left--;
			
		}
		// next
		current_active = new_active%lights->getNumLights();
	}
	while ( anim_pct >= 1 )
		anim_pct -= 1;
	while ( anim_pct < 0 )
		anim_pct += 1;
	
	if ( count_left < 0 )
	{
		// finished
		ofxOscMessage m;
		m.setAddress("/seq/end");
		Osc::sendMessage( m );
	}		
	
}
