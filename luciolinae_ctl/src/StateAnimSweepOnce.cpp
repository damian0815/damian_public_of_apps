/*
 *  StateAnimSweepOnce.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 05/06/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "StateAnimSweepOnce.h"
#include "SharedData.h"
#include "StateAnimSweeps.h"

const char* StateAnimSweepOnce::NAME = "state_sweep";

void StateAnimSweepOnce::enter()
{
	finished = false;
	pos = -0.1f;
	speed = 10.0f;
	
	sweep_dir = SharedData::getFloat("sweep_dir");
	
}



void StateAnimSweepOnce::update( float elapsed )
{
	float old_pos = pos;
	float this_speed = speed*(0.2f+pos*pos);
	float new_pos = old_pos + elapsed * this_speed;
	
	for ( float i = new_pos-0.5f; i <= new_pos; i+= 0.05f )
	{
		float power = (1-(new_pos-i)/0.5f);
		if ( sweep_dir == SWEEP_X )
			lights->illuminateCorridor( i, 0, 0, 1, power, 0.1f );
		else
			lights->illuminateCorridor( 0, i, 1, 0, power, 0.1f );
			
	}
	
	pos = new_pos;
}

void StateAnimSweepOnce::draw()
{
	float power = 1;
	lights->drawIlluminateCorridor( pos, 0, 0, 1, power, 0.05f );

}

