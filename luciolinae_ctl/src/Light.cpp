/*
 *  Light.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 19/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "Light.h"

// smallest brightness step size
static const float FLOAT_STEP_SIZE = 1.0f/4096;


Light::Light()
{
	brightness = 0; 
	target_brightness = 0; 
	last_brightness = 1; // force serial update 
	// decay to given pct of current brightness every second
	decay_factor = 0.3f; 
	was_pulsed = false;
}

void Light::setup( int _board_id, int _light_id, float _x, float _y )
{
	board_id = _board_id; 
	light_id = _light_id;
	x = _x;
	y = _y;
}

void Light::setup( ofxXmlSettings& data )
{
	x = data.getValue( "x", 0.0f );
	y = data.getValue( "y", 0.0f );
	board_id = data.getValue( "board_id", 0 );
	light_id = data.getValue( "light_id", 0 );	
	big = data.getValue( "big", false );
}

void Light::save( ofxXmlSettings& data )
{
	data.addValue( "x", x );
	data.addValue( "y", y );
	data.addValue( "board_id", board_id );
	data.addValue( "light_id", light_id );
	data.addValue( "big", big );
}


void Light::draw()
{
	ofNoFill();
	ofSetColor( 0xff, 0xff, 0xff, 0x08 );
	ofCircle( x*ofGetWidth(), y*ofGetHeight(), big?12:6 );
	ofFill();
	ofSetColor( 0x00, 0x00, 0xff, brightness*0xff );
	ofCircle( x*ofGetWidth(),y*ofGetHeight(), sqrtf(brightness)*(big?12:6) );
}


void Light::update(float elapsed )
{	
/*	if ( last_set_timer < 0.5f )
	{
		last_set_timer += elapsed;
		if ( last_set_timer >= 0.5f )
		{
			target_brightness = 0;
			decay_factor = 0.9f;
		}
	}*/
	// move toward target brightness
	if ( decay_factor > 0 && fabsf(brightness-target_brightness)>=0.5f*FLOAT_STEP_SIZE )
	{
//		if ( light_id == 0x0a && board_id == 0x10 )
//			printf("updating light %x:%x : was %8.6f ", board_id, light_id, brightness );
		brightness += powf(decay_factor,elapsed) * (target_brightness-brightness);
//		if ( light_id == 0x0a && board_id == 0x10 )
//			printf("now %8.6f (powf %8.6f)\n", brightness, powf(decay_factor, elapsed) );
	}
	else
	{
		brightness = target_brightness;
	}
}


void Light::pulse( float max_bright, /*float _decay_factor*/ float end_brightness )
{
	if ( max_bright > brightness )
	{
		brightness = max_bright;
		last_set_timer = 0;
	}
//	decay_factor = _decay_factor;
	
/*	if ( decay_factor > 0 )
		was_pulsed = true;
	else*/
		was_pulsed = false;
	
	target_brightness = end_brightness;
}

void Light::set( float bright ) 
{
	brightness = bright;
	target_brightness = bright;
	last_set_timer = 0;
	was_pulsed = false;
}

bool Light::needsSerial() const
{ 
	return (was_pulsed || fabsf(brightness-last_brightness)>=0.5f*FLOAT_STEP_SIZE);
}





