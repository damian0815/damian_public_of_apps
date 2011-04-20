/*
 *  Wasp.cpp
 *  iPhoneTouchAndAccelExample
 *
 *  Created by damian on 19/03/11.
 *  Copyright 2011 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "Wasp.h"

#include "ofMain.h"

#include "InputHistory.h"

void Wasp::setup(InputHistory* hist ) 
{ 
	input_history = hist;
	trail_time = ofRandomuf();
	// square distribution
	float max_speed_pre = ofRandomuf();
	max_speed_pre *= max_speed_pre;
	max_speed = 1000.0f + max_speed_pre*1000.0f;
	
	acceleration = ofRandom( 0.8f, 1.2f ) * max_speed * 5.0f;
	
}

void Wasp::setPosition( const ofVec2f& p )
{
	pos = p;
	target = pos;
}

void Wasp::update()
{
	float t = ofGetElapsedTimef()-trail_time*InputHistoryDuration;
	ofVec2f accel;
	if ( input_history->hasTouchAtTime( t ) )
	{
		// point toward target
		ofVec2f target = input_history->getTouchAtTime( t );
		accel = acceleration*(target-pos).normalized();
		/*
		// add some noise to the accelleration
		accel.x *= ofRandom( 0.3f, 1.0f );
		accel.y *= ofRandom( 0.3f, 1.0f );*/
	}
	else
	{
		// drift off randomly
		ofVec2f target = pos + vel * ofGetLastFrameTime();
		accel = acceleration*(target-pos).normalized();
		accel *= 0.5f;
		// add some noise to the accelleration
		accel.x *= ofRandom( -1.0f, 1.0f );
		accel.y *= ofRandom( -1.0f, 1.0f );
	}

	// update velocity
	vel += accel * ofGetLastFrameTime();
	// limit velocity
	float speed = vel.length();
	if ( speed > max_speed )
	{
		vel /= (speed/max_speed);
	}
	// update position
	pos += vel*ofGetLastFrameTime();
	
	/*trail_time += ofRandom( -0.2f, 0.2f )*ofGetLastFrameTime();
	if ( trail_time < 0.0f )
		trail_time = -trail_time;
	if ( trail_time > 1.0f )
		trail_time = 1.0f-trail_time;
	trail_time = min(max(0.0f,trail_time),1.0f);*/
}


void Wasp::draw()
{
	ofFill();
	// black
	ofSetColor( 0x000000 );
	ofCircle( pos.x, pos.y, 6 );
	// yellow
	ofSetHexColor( 0xffff00 );
	ofCircle( pos.x, pos.y, 2 );

	ofVec2f target = input_history->getTouchAtTime( ofGetElapsedTimef()-trail_time*InputHistoryDuration );
	ofSetColor( 0, 0, 0, 50 );
	ofLine( target.x, target.y, pos.x, pos.y );
}
