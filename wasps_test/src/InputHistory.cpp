/*
 *  InputHistory.cpp
 *  iPhoneTouchAndAccelExample
 *
 *  Created by damian on 19/03/11.
 *  Copyright 2011 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "InputHistory.h"

/// store 10s of movement

void InputHistory::addTouch( double time, ofVec2f pos )
{
	
	history.push_back( TimeTaggedPos( time, pos ) );
	while ( history.front().time < (time - InputHistoryDuration) )
		history.pop_front();
	
}

bool InputHistory::hasTouchAtTime( double time )
{
	if ( history.size() == 0 )
		return false;
	if ( time < history.front().time )
		return false;
	if ( time > history.back().time )
		return false;
	
	return true;
}

ofVec2f InputHistory::getTouchAtTime( double time )
{
	if ( history.size() == 0 )
		return ofVec2f(0,0);
	
	TimeTaggedPos* prev = &history[0];
	for ( int i=1; i<history.size(); i++ )
	{
		TimeTaggedPos* curr = &history[i];
		if ( curr->time >= time )
		{
			// interpolate
			double delta_t = curr->time - prev->time;
			double interp = (time - prev->time)/delta_t;
			ofVec2f result = prev->pos*(1.0-interp) + curr->pos*interp;
			return result;
		}
		else
			prev = curr;
	}

	return prev->pos;
	
}

void InputHistory::draw()
{
	ofSetHexColor( 0x808080 );
	ofNoFill();
	if ( history.size() > 0 ) 
	{
		ofBeginShape();
		ofCurveVertex( history[0].pos.x, history[0].pos.y );
		for ( int i=0;i <history.size(); i++ )
		{
			ofCurveVertex(history[i].pos.x, history[i].pos.y );
		}
		ofCurveVertex( history.back().pos.x, history.back().pos.y );
		ofCurveVertex( history.back().pos.x, history.back().pos.y );
		ofEndShape();
	}
}
