/*
 *  StateAnimBlipAll.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 05/06/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "StateAnimBlipAll.h"

const char* StateAnimBlipAll::NAME = "state_blipall";

void StateAnimBlipAll::update( float elapsed )
{

	timer -= elapsed;
	if ( timer < 0 && !blipped )
	{
		
		// blip everything
		for ( int i= 0;i<lights->getNumLights(); i++ )
			lights->pulse( i, 1, true );
		ofxOscMessage m;
		m.setAddress("/blip_all/trigger");
		Osc::sendMessage( m );
		timer = squaredRandom( 0.3f, 0.7f ); 
		blipped = true;
	}	
}