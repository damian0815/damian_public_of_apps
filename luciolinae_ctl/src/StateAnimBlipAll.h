/*
 *  StateAnimBlipAll.h
 *  luciolinae_ctl
 *
 *  Created by damian on 05/06/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once


#pragma once
#include "Util.h"
#include "Osc.h"

#include "StateAnimation.h"

class StateAnimBlipAll: public StateAnimation
{
public:
	StateAnimBlipAll( Lights* _lights ) : StateAnimation( _lights ) {};
	
	static const char* NAME;
	string getName() { return NAME; }
	
	void update( float elapsed );
	void draw() {};
	
	void enter() { 
		blipped = false;
		timer = SharedData::getFloat( "blipall_timer" );
		if ( timer < 0 )
		{
			timer = squaredRandom( 0.1f, 0.6f ); 
			SharedData::setFloat( "blipall_timer", timer );
		}
		else
		{
			timer *= squaredRandom( 0.5f, 2.0f );
		}
	}
	bool isFinished() { return blipped && timer < 0; }
	
private:

	bool blipped;
	float timer;
	int which;
	

};