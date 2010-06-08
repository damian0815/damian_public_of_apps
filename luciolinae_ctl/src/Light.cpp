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
	needs_count = 2;
	decay_factor = 10.0f; 
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
	ofCircle( x*ofGetWidth(), y*ofGetHeight(), 3+(big?9:3) );
	ofFill();
	ofSetColor( 0x00, 0x00, 0xff, sqrtf(brightness)*0xff );
	ofCircle( x*ofGetWidth(),y*ofGetHeight(), 3+sqrtf(brightness)*(big?9:3) );
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
	if ( decay_factor > 0 && fabsf(brightness-target_brightness)>=FLOAT_STEP_SIZE )
	{
//		if ( light_id == 0x0a && board_id == 0x10 )
//			printf("updating light %x:%x : was %8.6f ", board_id, light_id, brightness );
		brightness += decay_factor*elapsed * (target_brightness-brightness);
		brightness = max(0.0f,min(1.0f,brightness));
		if ( fabsf(brightness-target_brightness) < FLOAT_STEP_SIZE )
			needs_count = 3;
		else
			needs_count = 2;

//		if ( light_id == 0x0a && board_id == 0x10 )
//			printf("now %8.6f (powf %8.6f)\n", brightness, powf(decay_factor, elapsed) );
	}
	else if ( fabsf(brightness-target_brightness)>=FLOAT_STEP_SIZE )
	{
		brightness = target_brightness;
		needs_count = 2;
	}
}


void Light::pulse( float max_bright, /*float _decay_factor*/ float end_brightness )
{
	if ( int(max_bright*4096) != int(brightness*4096) )
		needs_count =2 ;
	if ( max_bright > brightness )
	{
		brightness = max_bright;
		last_set_timer = 0;
	}
	
	
	target_brightness = end_brightness;
}

void Light::set( float bright ) 
{
	if ( int(bright*4096) != int(brightness*4096) )
	{
		if ( bright == 0 )
			needs_count = 3;
		else	
			needs_count = 2;
	}
	brightness = bright;
	target_brightness = bright;
	last_set_timer = 0;
}

bool Light::needsSerial() const
{ 
	return (fabsf(brightness-last_brightness)>=0.5f*FLOAT_STEP_SIZE) 
		|| needs_count > 0;
}

void Light::resetNeedsSerial()
{ 
	last_brightness = brightness; 
	if ( needs_count > 0 )
		needs_count --;
}






