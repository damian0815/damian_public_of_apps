/*
 *  AnimPositionCalibrate.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 18/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "AnimPositionCalibrate.h"
#include "NearestPointOnLineToPoint.h"

const char* AnimPositionCalibrate::NAME = "PositionCalibrate";

AnimPositionCalibrate::AnimPositionCalibrate( Lights* _lights )
: Animation( _lights )
{
	position_pct = 0;
	current_id = 0;
	mode = MODE_SWEEP_Y;
	phase = 0;
}


void AnimPositionCalibrate::keyPressed( int k )
{
	switch( k )
	{
		case OF_KEY_RIGHT:
			current_id++;
			phase = 0;
			if ( current_id >= lights->getNumLights() )
				current_id = 0;
			break;
		case OF_KEY_LEFT:
			current_id--;
			phase = 0;
			if ( current_id < 0 )
				current_id = lights->getNumLights()-1;
			break;
		case OF_KEY_RETURN:
			if ( mode == MODE_SWEEP_Y )
				lights->setLightPosition( current_id, lights->getLight( current_id ).getX(), position_pct );
			else if ( mode == MODE_SWEEP_X )
				lights->setLightPosition( current_id, position_pct, lights->getLight( current_id ).getY() );
			else
			{
				// get the nearest point on the line
				const Light& light = lights->getLight( current_id );
				ofxVec3f p( 0, position_pct );
				ofxVec3f d( sqrtf(2.0f), -sqrtf(2.0f) );
				ofxVec3f light_pos( light.getX(), light.getY() );
				ofxVec3f new_pos = NearestPointOnLineToPoint::calculateNearestPoint( p, p+d, light_pos );
				lights->setLightPosition( current_id, new_pos.x, new_pos.y );
			}
			break;
		case 'B':
			lights->toggleLightIsBig( current_id );
			break;
		case 'x':
			mode = MODE_SWEEP_X;
			break;
		case 'y':
			mode = MODE_SWEEP_Y;
			break;
		case 'z':
			mode = MODE_SWEEP_DIAG;
			break;
			
		default:
			break;
			
	}			
	
}

void AnimPositionCalibrate::mouseMoved( int x, int y ) 
{
	if ( mode == MODE_SWEEP_X )
		position_pct = float(x)/ofGetWidth();
	else if ( mode == MODE_SWEEP_Y )
		position_pct = float(y)/ofGetHeight();
	else
		position_pct = float(x)/ofGetWidth() + float(y)/ofGetHeight();
}

void AnimPositionCalibrate::draw()
{

	// figure out which lights should be on
	float x, y, dx, dy;
	if ( mode == MODE_SWEEP_Y )
	{
		x = 0;
		y = position_pct;
		dx = 1;
		dy = 0;
	}
	else if ( mode == MODE_SWEEP_X )
	{
		x = position_pct;
		y = 0;
		dx = 0;
		dy = 1;
	}
	else /* MODE_SWEEP_DIAG */ 
	{
		x = 0;
		y = position_pct;
		dx = sqrtf( 2 );
		dy = -sqrtf( 2 );
	}
	
	//printf("illuminating corridor: %f,%f  %f,%f\n", x, y, dx, dy );
	lights->drawIlluminateCorridor( x, y, dx, dy, 0.1f, 0.1f );

	// draw current light highlight
	const Light& current_light = lights->getLight( current_id );
	ofSetColor( 0x50, 0x30, 0x30 );
	ofNoFill();
	ofCircle( current_light.getX()*ofGetWidth(), current_light.getY()*ofGetHeight(), 15 );
}

void AnimPositionCalibrate::update( float elapsed )
{
	
	// figure out which lights should be on
	float x, y, dx, dy;
	if ( mode == MODE_SWEEP_Y )
	{
		x = 0;
		y = position_pct;
		dx = 1;
		dy = 0;
	}
	else if ( mode == MODE_SWEEP_X )
	{
		x = position_pct;
		y = 0;
		dx = 0;
		dy = 1;
	}
	else /* MODE_SWEEP_DIAG */ 
	{
		x = 0;
		y = position_pct;
		dx = sqrtf( 2 );
		dy = -sqrtf( 2 );
	}
	
	//printf("illuminating corridor: %f,%f  %f,%f\n", x, y, dx, dy );
	lights->illuminateCorridor( x, y, dx, dy, 0.1f, 0.1f );
	
	// update current light
	phase += 10.0f*elapsed;
	while ( phase > PI/2 )
		phase -= PI;
	float bright = 0.5f*cosf(phase);
	bright *= bright;
	lights->set( current_id, bright );
	
}
