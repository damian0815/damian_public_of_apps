/*
 *  AnimSweep.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 19/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */



#include "AnimSweep.h"


const char* AnimSweep::NAME = "Sweep";

void AnimSweep::update( float elapsed )
{
	position_pct += speed*elapsed;
	if ( position_pct > 1 )
	{
		position_pct = 1;
		speed = -speed;
	}
	else if ( position_pct < 0 )
	{
		position_pct = 0;
		speed = -speed;
	}
/*	while ( position_pct < 0 )
		position_pct += 1;
	while ( position_pct > 1 )
		position_pct -= 1;*/
	
	// figure out which lights should be on
	float x, y, dx, dy;
	if ( mode == MODE_SWEEP_Y )
	{
		x = 0;
		y = position_pct*1.2f-0.1f;
		dx = 1;
		dy = 0;
	}
	else if ( mode == MODE_SWEEP_X )
	{
		x = position_pct*1.2f-0.1f;
		y = 0;
		dx = 0;
		dy = 1;
	}
	else /* MODE_SWEEP_DIAG */ 
	{
		x = 0;
		y = (position_pct*1.2f-0.1f)*2;
		dx = sqrtf( 2 );
		dy = -sqrtf( 2 );
	}
	
	//printf("illuminating corridor: %f,%f  %f,%f\n", x, y, dx, dy );
	lights->illuminateCorridor( x, y, dx, dy, 0.1f, 0.05f );
}


void AnimSweep::keyPressed( int k )
{
	switch( k )
	{
		case 'x':
			mode = MODE_SWEEP_X;
			break;
		case 'y':
			mode = MODE_SWEEP_Y;
			break;
		case 'z':
			mode = MODE_SWEEP_DIAG;
			break;
		case 's':
			speed *= 1.2f;
			break;
		case 'S':
			speed /= 1.2f;
			break;
			
		default:
			break;
	}
}

void AnimSweep::draw()
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
		y = position_pct*2;
		dx = sqrtf( 2 );
		dy = -sqrtf( 2 );
	}
	
	//printf("illuminating corridor: %f,%f  %f,%f\n", x, y, dx, dy );
	lights->drawIlluminateCorridor( x, y, dx, dy, 0.1f, 0.1f );
}