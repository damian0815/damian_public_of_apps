/*
 *  AnimRotate.cpp
 *  serialExample
 *
 *  Created by damian on 07/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "AnimRotate.h"

static const int STEPS=16;

AnimRotate::AnimRotate( Lights* _lights ) 
:	Animation( _lights )
{
	anim_pct = 0.0f;
	current_active = -1;
	speed = 1.0f;
}

void AnimRotate::update( float elapsed )
{
	float speed_factor = 6;
	float min_speed = -3.0f;
	float real_speed = (speed>1?2-speed:speed) * speed_factor + min_speed;
	speed += 0.1f*elapsed;
	if ( speed >=2 )
		speed = 0;
	
	// update pct
	anim_pct += elapsed * real_speed;
	// calculate new active
	int new_active = anim_pct*STEPS;
	if ( new_active != current_active )
	{
		// we might jump more than one
		int dir = (real_speed>=0) ? 1:-1 ;
		int first, last;
		printf("current_active %2i, new_active %2i, dir %i; ", current_active, new_active, dir );
		// have we gone full circle?
		if ( abs(new_active-current_active)>STEPS )
		{
			// yes - then pulse everything
			last = new_active;
			first = last - dir*(STEPS-1);
		}
		else 
		{
			last = new_active;
			first = current_active + dir;
			if ( first < 0 )
				first += STEPS;
		}
		printf("pulsing from %i to %i\n", first, last );
		
		// pulse the new one(s)
		int count = abs(last-first)+1;
		for ( int i=0; i<count; i++ )
		{
			int which = first + dir*i;
			printf("pulsing %i\n", which%STEPS );
			lights->pulse( which%STEPS, 1.0 );			
		}
		// next
		current_active = new_active%STEPS;
	}
	while ( anim_pct >= 1 )
		anim_pct -= 1;
	while ( anim_pct < 0 )
		anim_pct += 1;
		
}
