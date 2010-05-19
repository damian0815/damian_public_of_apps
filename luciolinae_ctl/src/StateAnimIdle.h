/*
 *  StateAnimIdle.h
 *  luciolinae_ctl
 *
 *  Created by damian on 19/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "StateAnimation.h"

class StateAnimIdle : public StateAnimation
{
public:
	StateAnimIdle( Lights* _lights ) : StateAnimation( _lights ) {};
	
	static const char* NAME;
	string getName() { return NAME; }
	
	void update( float elapsed );
	void draw();
	
	void enter();
	bool isFinished();
	
private:
	
	float timer;
	
};

