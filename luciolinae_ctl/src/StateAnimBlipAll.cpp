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
		set<int> blip_lights;
		while( blip_lights.size() < 5 )
		{
			int add = ofRandom(0, lights->getNumLights()*0.99999f );
			if ( blip_lights.find( add ) == blip_lights.end() )
				blip_lights.insert( add );
		}
		
		for ( set<int>::iterator it = blip_lights.begin(); it != blip_lights.end() ;++it )
		{
			lights->pulse( *it, 1, true );
		}
		ofxOscMessage m;
		m.setAddress("/blip_all/trigger");
		Osc::sendMessage( m );
		timer = 0.3f;
		blipped = true;
	}	
}
