/*
 *  AnimKapelica.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 15/08/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "AnimKapelica.h"

#include "Osc.h"

const char* AnimKapelica::NAME = "Kapelica";

#include "DelaunayPulse.h"

// how long to wait after triggering a delaunay down before pulsing
static const float DELAUNAY_DOWN_MIN = 1.0f;
static const float DELAUNAY_DOWN_MAX = 2.5f;
// how long to wait after pulsing to wait before triggering a delaunay up
static const float DELAUNAY_UP_MIN = 0.1f;
static const float DELAUNAY_UP_MAX = 0.5f;
// how long to stay doing nothing
static const float IDLE_MIN = 2.0f;
static const float IDLE_MAX = 20.0f;
// time scalar
static const float MIN_TIME_SCALAR = 0.7f;
static const float MAX_TIME_SCALAR = 1.5f;

AnimKapelica::AnimKapelica( Lights* _lights ) 
:	Animation( _lights )
{
	for ( int i=0; i<4; i++ )
	{
		units.push_back( KapelicaUnit( i ) );
	}
	
}

void AnimKapelica::update( float elapsed )
{
	for ( int i=0; i<units.size(); i++ )
	{
		units[i].update( lights, elapsed );
	}
}


KapelicaUnit::KapelicaUnit( int _id )
{ 
	id = _id; 
	time_scalar = ofRandom( MIN_TIME_SCALAR, MAX_TIME_SCALAR ); 
	timer = 0; 
	state = KU_DELAUNAY_DOWN; 
};


void KapelicaUnit::update( Lights* lights, float elapsed )
{
	timer -= elapsed;
	
	switch( state )
	{
		case KU_IDLE:
		{
			if( timer < 0.0f )
			{
				state = KU_DELAUNAY_DOWN;
				timer = time_scalar*ofRandom( DELAUNAY_DOWN_MIN, DELAUNAY_DOWN_MAX );
				const Light& which_light = lights->getBigLight( id );
				DelaunayPulses::getInstance()->addPulseIn( lights->getBigLightIndex( id ), 0.5f, time_scalar, 0.5f, 
														  lights->getBoardDelaunay( which_light.getBoardId() ) );
			}
			break;
		}
			
		case KU_DELAUNAY_DOWN:
		{
			if ( timer < 0.0f )
			{
				state = KU_PULSE;
				// pulse the big light
				int which = lights->getBigLightIndex( id );
				
				lights->pulse( which, 1.0f, true );
				timer = time_scalar*ofRandom( DELAUNAY_UP_MIN, DELAUNAY_UP_MAX );
				
				ofxOscMessage m;
				m.setAddress( "/blip/target" );
				// add big light index (0..num_big_lights)
				m.addIntArg( id );
				const Light& big_light = lights->getLight( which );
				// add x/y position
				m.addFloatArg( big_light.getX() );
				m.addFloatArg( big_light.getY() );
				// send the message
				Osc::getInstance()->sendMessage( m );

				// trigger
				m.clear();
				m.setAddress("/blip/trigger");
				m.addFloatArg( id );
				Osc::sendMessage( m );
				
			
			}				
			break;
		}

		case KU_PULSE:
		{
			if ( timer < 0.0f )
			{
				state = KU_DELAUNAY_UP;
				const Light& which_light = lights->getBigLight( id );
				DelaunayPulses::getInstance()->addPulseOut( lights->getBigLightIndex( id ), 0.5f, time_scalar, 0.5f, 
														  lights->getBoardDelaunay( which_light.getBoardId() ) );
				timer = time_scalar*ofRandom( IDLE_MIN, IDLE_MAX );
			}
			break;
		}

			
		case KU_DELAUNAY_UP:
		{
			if ( timer < 0.0f )
			{
				state = KU_IDLE;
				timer = 0.0f;
				time_scalar = ofRandom (MIN_TIME_SCALAR, MAX_TIME_SCALAR );
			}
			break;
		}
	}			
	
}
