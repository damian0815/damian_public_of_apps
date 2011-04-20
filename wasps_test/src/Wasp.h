/*
 *  Wasp.h
 *  iPhoneTouchAndAccelExample
 *
 *  Created by damian on 19/03/11.
 *  Copyright 2011 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "ofVec3f.h"

class InputHistory;

class Wasp
{
public:
	
	void setup(InputHistory* hist );
	
	void setPosition( const ofVec2f& p );
	void update();
	void draw();
	
private:
	
	InputHistory* input_history;
	
	ofVec2f vel;
	ofVec2f pos;
	
	ofVec2f target;
	
	float trail_time;
	float max_speed;
	float acceleration;
	
};
