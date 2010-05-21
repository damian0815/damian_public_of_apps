/*
 *  Util.h
 *  luciolinae_ctl
 *
 *  Created by damian on 21/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once
#include "ofMain.h"

inline float squaredRandom( float first, float second )
{
	// get the random pct
	float r = ofRandomuf();
	r*=r;
	
	// so then
	return first + r*(second-first);
	
}
