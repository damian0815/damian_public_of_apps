/*
 *  StateAnimStartBlip.h
 *  luciolinae_ctl
 *
 *  Created by damian on 19/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "StateAnimation.h"

class StateAnimStartBlip : public StateAnimation
{
public:
	StateAnimStartBlip( Lights* _lights ) : StateAnimation( _lights ) {};
	
	static const char* NAME;
	string getName() { return NAME; }
	
	// all work happens here
	void enter();
	// immediately exit
	bool isFinished() { return true; }
	
private:
	
};

