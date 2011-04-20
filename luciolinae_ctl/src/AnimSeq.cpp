/*
 *  AnimSeq.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 18/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "AnimSeq.h"

const char* AnimSeq::NAME = "Seq";

AnimSeq::AnimSeq( Lights* _lights ) 
:	Animation( _lights )
{
	anim_pct = 0.0f;
	current_active = -1;
	speed = 0.2f;
}

void AnimSeq::update( float elapsed )
{
	float real_speed = speed*16.0f*1.0f/lights->getNumLights();
	
	// update pct
	anim_pct += elapsed * real_speed;
	// calculate new active
	int new_active = anim_pct*lights->getNumLights();
    //int new_active = 3*16 + anim_pct*16;
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
			lights->pulse( which%lights->getNumLights(), 1.0f );
		}
		// next
		current_active = new_active%lights->getNumLights();
	}
	while ( anim_pct >= 1 )
		anim_pct -= 1;
	while ( anim_pct < 0 )
		anim_pct += 1;
	
}
