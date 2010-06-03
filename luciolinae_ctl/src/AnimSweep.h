/*
 *  AnimSweep.h
 *  luciolinae_ctl
 *
 *  Created by damian on 19/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "Animation.h"

class AnimSweep : public Animation
{
public:
	AnimSweep(Lights* _lights) : Animation( _lights ) { position_pct = 0; mode = MODE_SWEEP_Y; speed = 2.0f; }

	static const char* NAME;
	
	void update( float elapsed );
	void draw();
	
	void keyPressed( int k );
	
private:
	float position_pct;
	float speed;
	
	typedef enum _MODE { 
		MODE_SWEEP_X,
		MODE_SWEEP_Y,
		MODE_SWEEP_DIAG } Mode;

	Mode mode;


};