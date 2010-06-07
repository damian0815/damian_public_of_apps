/*
 *  StateAnimSweepOnce.h
 *  luciolinae_ctl
 *
 *  Created by damian on 05/06/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "StateAnimation.h"

class StateAnimSweepOnce : public StateAnimation
{
public:
	StateAnimSweepOnce( Lights* _lights ) : StateAnimation( _lights ) { speed = 3.0f; };
	
	static const char* NAME;
	string getName() { return NAME; }
	
	void draw();
	
	// all work happens here
	void enter();
	// immediately exit
	bool isFinished() { return pos > 1.1f; }
	
	// update
	void update( float elapsed );
	

	
private:

	bool finished;
	float pos;
	float speed;
	int sweep_dir;
};
