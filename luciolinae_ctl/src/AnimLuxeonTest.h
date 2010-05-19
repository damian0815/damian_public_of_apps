/*
 *  AnimLuxeonTest.h
 *  luciolinae_ctl
 *
 *  Created by damian on 17/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "Animation.h"

class AnimLuxeonTest: public Animation
{
public:
	AnimLuxeonTest( Lights* _lights );
	
	void update( float elapsed );
	void draw() {};
	
protected:
	
	float timer;
	int board;
	int current_light;
	
};

