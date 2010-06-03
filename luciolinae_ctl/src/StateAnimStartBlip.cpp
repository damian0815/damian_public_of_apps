/*
 *  StateAnimStartBlip.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 19/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "StateAnimStartBlip.h"
#include "Osc.h"

const char* StateAnimStartBlip::NAME = "state_start_blip";

void StateAnimStartBlip::enter()
{
	// choose a big light to blip
	assert( lights->getNumBigLights() > 0 );

#ifdef ALWAYS_THE_SAME
	int which_big_light = 0;
#else
	int which_big_light = ofRandom( 0, 0.999f*lights->getNumBigLights() );
	if ( SharedData::getFloat( "blip_target_must_change" ) > 0 )
	{	
		if ( lights->getNumBigLights() > 1 )
		{
			int prev_big_light = lights->getLightIndexForBig( SharedData::getFloat( "blip_target" ) );
			// avoid an infinite loop in case big lights have changed
			if ( prev_big_light != -1 )
				while ( which_big_light == prev_big_light )
					which_big_light = ofRandom( 0, 0.999f*lights->getNumBigLights() );
		}
	}
#endif
	
	int which = lights->getBigLightIndex( which_big_light );
	SharedData::setFloat( "blip_target", which );
	SharedData::setFloat( "blip_count", 0 );
	
	ofxOscMessage m;
	m.setAddress( "/blip/target" );
	// add big light index (0..num_big_lights)
	m.addIntArg( which_big_light );
	const Light& big_light = lights->getLight( which );
	// add x/y position
	m.addFloatArg( big_light.getX() );
	m.addFloatArg( big_light.getY() );
	// send the message
	Osc::getInstance()->sendMessage( m );
}
