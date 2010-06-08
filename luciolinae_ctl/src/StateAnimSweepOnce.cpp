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
#include "Util.h"

const char* StateAnimSweepOnce::NAME = "state_sweep";

void StateAnimSweepOnce::enter()
{
	finished = false;
	pos = 0;
	speed = squaredRandom( 1.0f, 2.0f );
	// sometimes we will abort early
	/*endpoint = ofRandomuf();
	endpoint *= endpoint;
	endpoint *= endpoint;
	endpoint = 1.0f-endpoint;*/
	endpoint = 1.0f;
	
	float sweep_dir_rand = ofRandomuf();
	if ( sweep_dir_rand <= 0.7f )
		sweep_dir = SWEEP_X;
	else if ( sweep_dir_rand <= 0.9f )
		sweep_dir = SWEEP_Y;
	else 
		sweep_dir = SWEEP_DIAG;
	
	reverse = ofRandomuf()>0.5f;
	
}



void StateAnimSweepOnce::update( float elapsed )
{
	float old_pos = pos;
	float this_speed = speed*(0.2f+pos*pos);
	float new_pos = old_pos + elapsed * this_speed;
	if ( new_pos > endpoint )
		lights->clear();
	else
	{
		
		for ( float i = new_pos-0.5f; i <= new_pos; i+= 0.05f )
		{
			float pos_pct = reverse?1-i:i;
			pos_pct *= 1.2f;
			pos_pct -= 0.1f;
			static const float ROOT_2 = sqrtf( 2.0f );
			float power = (1-(new_pos-i)/0.5f);
			power = max(0.0f,min(1.0f,power));
			power *= 0.3f;
			if ( sweep_dir == SWEEP_X )
				lights->illuminateCorridor( pos_pct, 0, 0, 1, power, 0.1f );
			else if ( sweep_dir == SWEEP_Y )
				lights->illuminateCorridor( 0, pos_pct, 1, 0, power, 0.1f );
			else
			{
				lights->illuminateCorridor( 0, (pos_pct*1.2f-0.1f)*2, ROOT_2, ROOT_2, power, 0.1f );
			}
				
		}
	}	
	pos = new_pos;
}

void StateAnimSweepOnce::draw()
{
	float power = 1;

	static const float ROOT_2 = sqrtf( 2.0f );
	if ( sweep_dir == SWEEP_X )
		lights->drawIlluminateCorridor( reverse?1-pos:pos, 0, 0, 1, power, 0.1f );
	else if ( sweep_dir == SWEEP_Y )
		lights->drawIlluminateCorridor( 0, reverse?1-pos:pos, 1, 0, power, 0.1f );
	else
	{
		float pos_pct = reverse?1-pos:pos;
		lights->drawIlluminateCorridor( 0, (pos_pct*1.2f-0.1f)*2, ROOT_2, ROOT_2, power, 0.1f );
	}
	
}

