/*
 *  StateAnimGather.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 20/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "StateAnimGather.h"
#include "DelaunayPulse.h"
#include "Util.h"



const char* StateAnimGather::NAME = "state_gather";

void StateAnimGather::update( float elapsed )
{
	timer -= elapsed;

	if ( timer < 0 )
	{
		// start a pulse inward
		int which = SharedData::getFloat( "blip_target" );
		DelaunayPulses::getInstance()->addPulseIn(which, 0.2f, 0.7f );
		// decrement counter
		count--;
		if ( count > 0 )
			// should we continue?
			timer = squaredRandom( 0.1f, 0.5f ); 
		// final pause
		else if ( count == 0 )
			timer = squaredRandom( 2.0f, 4.0f );
		
	}
}