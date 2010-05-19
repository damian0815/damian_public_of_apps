/*
 *  StateAnimStartBlip.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 19/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "StateAnimStartBlip.h"

const char* StateAnimStartBlip::NAME = "state_start_blip";

void StateAnimStartBlip::enter()
{
	// choose a big light to blip
	assert( lights->getNumBigLights() > 0 );
	int which = lights->getBigLightIndex( ofRandom( 0, 0.999f*lights->getNumBigLights() ) );
	SharedData::setFloat( "blip_target", which );
}
