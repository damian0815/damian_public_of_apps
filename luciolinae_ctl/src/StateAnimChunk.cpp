/*
 *  StateAnimChunk.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 25/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "StateAnimChunk.h"
#include "Util.h"
#include "Osc.h"

const char* StateAnimChunk::NAME = "chunk";

void StateAnimChunk::enter()
{
	chunks = squaredRandom( 2, 10 );
	total_chunks = chunks;
	chunk_time = squaredRandom( 0.3f, 1.3f );
	timer = 0;
	ofxOscMessage m;
	m.setAddress("/chunks/setup");
	m.addFloatArg( total_chunks );
	m.addFloatArg( chunk_time );
	Osc::sendMessage( m );
}

void StateAnimChunk::update( float elapsed )
{
	timer -= elapsed;
	if ( timer < 0 && chunks > 0 )
	{
		// illuminate a chunk
		float w = ofRandom( 0.2, 0.5+(float(chunks)/total_chunks)*0.5 );
		float h = ofRandom( 0.2, 0.5+(float(chunks)/total_chunks)*0.5 );
		float x = ofRandom( 0, 1-w );
		float y = ofRandom( 0, 1-h );
		float power = 0.5f;
		lights->illuminateRect( x, y, w, h, power );
		
		ofxOscMessage m;
		m.setAddress("/chunks/trigger");
		m.addFloatArg( x );
		m.addFloatArg( y );
		m.addFloatArg( w );
		m.addFloatArg( h );
		m.addFloatArg( power );
		m.addFloatArg( total_chunks-chunks );
		Osc::sendMessage( m );

		chunks--;		
		timer += chunk_time;
	}
}


