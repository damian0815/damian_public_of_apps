/*
 *  StateAnimIdle.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 19/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "StateAnimIdle.h"

const char* StateAnimIdle::NAME = "state_idle";
bool StateAnimIdle::should_reset_random = false;

void StateAnimIdle::enter()
{
#ifdef ALWAYS_THE_SAME
	ofSeedRandom( 12345 );
#endif
	// set a random timer
	timer = ofRandom( 2.0f, 8.0f );
}


void StateAnimIdle::update( float elapsed )
{
	timer -= elapsed;
}

bool StateAnimIdle::isFinished() 
{
	return timer < 0; 
}


void StateAnimIdle::draw()
{
	ofSetColor( 0xff, 0xff, 0xff, 0x80 ); 
	char buf[1024];
	sprintf( buf, "timer: %8.3f", timer );
	ofDrawBitmapString( buf, 10, ofGetHeight()-50 );
}
