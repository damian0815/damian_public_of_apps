/*
 *  Activators.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 13/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "Activators.h"

void Activators::setup( int count, Lights* _lights )
{
	lights = _lights;
	activators.resize( count );
	for ( int i=0; i<count;i ++ )
	{
		activators[i].setup( ofRandom( 0,1 ), ofRandom(0,1), ofRandom(0.1f, 0.3f) );
	}
	timer = 0.0f;
}

void Activators::update( float elapsed )
{
//	printf("frame ~~\n");
	// update
	for ( int i=0; i<activators.size();i ++ )
	{
		// update
		activators[i].update( elapsed );
		// apply to lights
		for ( int j=0; j<lights->getNumLights(); j++ )
		{
			// get light
			const Light& light = lights->getLight( j );
			
			// calculate pulse brightness
			float brightness = activators[i].getActivationEnergy( light.getX(), light.getY() );
			
			// set
			if ( brightness > 0 )
			{
				lights->pulse( j, brightness, 0 );
			}
		}
	}
	
	// new movement?
	timer -= elapsed;
	if ( timer < 0 )
	{
		// reset timer square random distribution between 0.2f and 2.0f
		timer = ofRandom( sqrtf(0.2f), sqrtf(2.0f ) );
		timer *= timer;
		
		// pick a random one
		int which = ofRandom( 0, activators.size() );
		// pick a random target
		float x = ofRandom( 0, 1 );
		float y = ofRandom( 0, 1 );
		// speed with squared distribution
		float speedPct = ofRandomuf();
		speedPct *= speedPct;
		// invert so faster is more likely
		speedPct = 1.0f-speedPct;
		float speed = 0.3f + speedPct*10.0f;
		// go
		if ( which < activators.size() )
			activators[which].moveTo( x, y, speed );
	}
	
	

}

void Activators::draw()
{
	for ( int i=0; i<activators.size();i ++ )
	{
		activators[i].draw();
	}
}
