/*
 *  AnimPositionCalibrate.h
 *  luciolinae_ctl
 *
 *  Created by damian on 18/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "Animation.h"

class AnimPositionCalibrate : public Animation
{
public:
	
	AnimPositionCalibrate( Lights* _lights );
	
	static const char* NAME;
	
	void update( float elapsed );
	void draw();

	void keyPressed ( int key );
	void mouseMoved( int x, int y );
	
private:
	
	typedef enum _MODE { 
		MODE_SWEEP_X,
		MODE_SWEEP_Y,
		MODE_SWEEP_DIAG } Mode;
	
	Mode mode;

	float position_pct;

	int current_id;
	float phase;
	
};
