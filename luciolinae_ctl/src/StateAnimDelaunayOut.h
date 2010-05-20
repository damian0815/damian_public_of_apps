/*
 *  StateAnimDelaunayOut.h
 *  luciolinae_ctl
 *
 *  Created by damian on 20/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "StateAnimation.h"
#include "DelaunayPulseOut.h"

class StateAnimDelaunayOut : public StateAnimation
{
public:
	StateAnimDelaunayOut( Lights* _lights ) : StateAnimation( _lights ) {};
	
	static const char* NAME;
	string getName() { return NAME; }
	
	void update( float elapsed ){};
	void draw() {};
	
	void enter() { 
		// blip the appropriate light
		int from_which = SharedData::getFloat( "blip_target" );
		DelaunayPulses::getInstance()->addPulseOut( from_which, 0.2f );
	}
	bool isFinished() { return true; }
	
private:
	
};
