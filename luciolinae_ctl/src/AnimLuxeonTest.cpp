/*
 *  AnimLuxeonTest.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 17/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "AnimLuxeonTest.h"

extern void delayUs( unsigned long us );

static const int BLACK_TIME = 3.0f;
static const int ON_TIME = 0.02f;

AnimLuxeonTest::AnimLuxeonTest( Lights* _lights ) 
:Animation( _lights )
{
	board = 0x10;
	current_light = 0;
	
	timer = BLACK_TIME;
}

void AnimLuxeonTest::update( float elapsed )
{
	timer -= elapsed;
	if ( timer >= 0 )
	{
	}
	else if ( timer < 0  )
	{
		printf("turning on %i\n", current_light);
		lights->set( current_light, 1.0f );
		lights->flush();
		delayUs( 50 );
		printf("turning off %i\n", current_light);
		lights->set( current_light, 0.0f );
		lights->flush();
		current_light++;
		if ( current_light > 1 )
			current_light =0;
		timer = BLACK_TIME;
	}
}
