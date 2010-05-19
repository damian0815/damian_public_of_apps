/*
 *  Activators.h
 *  luciolinae_ctl
 *
 *  Created by damian on 13/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "Activator.h"
#include "Lights.h"

class Activators
{
public:	
	/// make count activators, to apply to given lights
	void setup( int count, Lights* _lights );
	
	void update( float elapsed );
	
	void draw();
	
	
private:
	
	vector<Activator> activators;
	
	Lights* lights;
	
	float timer;
	
};

