/*
 *  Breathing.h
 *  luciolinae_ctl
 *
 *  Created by damian on 20/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once
#include "Animation.h"

class Breathing 
{
public:
	Breathing() { timer = 1; lung_fullness=0; breath_time = 0; };
	void update( float elapsed );	
private:
	float timer;
	float lung_fullness;
	float breath_time;
};