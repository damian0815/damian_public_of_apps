/*
 *  StateAnimation.h
 *  luciolinae_ctl
 *
 *  Created by damian on 19/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once
#include "Animation.h"
#include "SharedData.h"

class StateAnimation: public Animation
{
public:
	
	StateAnimation(Lights* _lights): Animation( _lights ){};

	virtual string getName() { return "StateAnimation"; }
	
	virtual void update( float elapsed ) {};
	virtual void enter() {};
	virtual bool isFinished() { return false; };
	
protected:
	
	// return a squared random number between first and second inclusive, favouring first
	static float getSquaredRandom( float first, float second );

};
