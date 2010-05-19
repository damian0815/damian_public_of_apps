/*
 *  StateAnimation.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 19/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "StateAnimation.h"

float StateAnimation::getSquaredRandom( float first, float second )
{
	// get the random pct
	float r = ofRandomuf();
	r*=r;
	
	// so then
	return first + r*(second-first);
	
}
