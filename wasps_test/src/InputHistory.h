/*
 *  InputHistory.h
 *  iPhoneTouchAndAccelExample
 *
 *  Created by damian on 19/03/11.
 *  Copyright 2011 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include <deque>

static const float InputHistoryDuration = 0.2f;

class InputHistory
{
public:

	void addTouch( double time, ofVec2f pos );
	
	bool hasTouchAtTime( double time );
	ofVec2f getTouchAtTime( double time );
	
	void draw();
	
private:
	
	struct TimeTaggedPos {
		TimeTaggedPos( double t, const ofVec2f& p ) { time = t; pos = p; }
		double time;
		ofVec2f pos;
	};
	deque< TimeTaggedPos > history;
	
};

