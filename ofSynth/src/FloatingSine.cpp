/*
 *  FloatingSine.cpp
 *  ofSynth
 *
 *  Created by damian on 12/01/11.
 *  Copyright 2011 frey damian@frey.co.nz. All rights reserved.
 *
 */


#include "FloatingSine.h"

static const float DISTANCE_MUL = 50.0f;;

static const float PENTATONIC[5] = { 0, 2, 5, 7, 10 };


float midiToFrequency( float midiNote )
{
	return 440.0f*powf(2.0f,(midiNote-60.0f)/12.0f);
}

void FloatingSine::setup( ofSoundMixer* mixer, vector<FloatingSine*> * n )
{
	neighbours = n;

//	const float E = 329.6f;
//	float baseFrequency = E*powf(2.0f,int(ofRandom( -2, 2 )));
	int whichPentatonic = ofRandom( 0, 4.99999f );
	float midiNote = 36 + PENTATONIC[whichPentatonic];
	float baseFrequency = midiToFrequency( midiNote );
	ofLog(OF_LOG_NOTICE, "tone %x: freq %f", &tone, baseFrequency );
	
	
	frequency = baseFrequency;
	tone.setFrequency( frequency );
	if ( ofRandomuf() > 0.5f ) 
		tone.setSawtoothWaveform();
	volume.addInputFrom( &tone );
	volume.setVolume( 4.0f/n->size() );
	mixer->addInputFrom( &volume );
	
	
	velocity = 0;
	
	position.set( ofRandom( 3.0f*ofGetWidth()/8, 5.0f*ofGetWidth()/8 ), ofRandom( 3.0f*ofGetHeight()/8, 5.0f*ofGetHeight()/8 ) );
	
	
	if ( neighbours->size() >= 3 )
	{
		buddy = ofRandomuf()*0.99999f*neighbours->size();
		// avoid selecting ourselves as the buddy
		while ( n->at( buddy ) == this )
		{
			buddy = (buddy+1)%neighbours->size();
		}
		enemy = ofRandomuf()*0.99999f*neighbours->size();
		// avoid selecting ourselves as the buddy
		while ( n->at( enemy ) == this || buddy == enemy )
		{
			enemy = (enemy+1)%neighbours->size();
		}
	}
	else 
	{
		buddy = 0; 
		enemy = 0;
	}
}



void FloatingSine::update( )
{
	// update the position
	position += velocity*ofGetLastFrameTime();
	
	// damp the velocity
	velocity *= powf(0.3f,ofGetLastFrameTime());


	ofVec2f delta = neighbours->at( buddy )->position - position;
	float distance = delta.length();
	ofVec2f deltaNorm = delta/distance;
	float distanceUnits = distance/DISTANCE_MUL;
	
	// calculate distance in terms of shells
	int whichPentatonic = distanceUnits;
	float remainder = distanceUnits - whichPentatonic;
	if ( remainder > 0.5f )
	{
		whichPentatonic += 1.0f;
		remainder = 1.0f-remainder;
	}
	// update frequency
	
	float midiNote = 48;
	while ( whichPentatonic>=5 )
	{
		midiNote += 12; 
		whichPentatonic -= 5;
	}
	midiNote += PENTATONIC[whichPentatonic%5];
	tone.setFrequency( midiToFrequency( midiNote + 0.5f*remainder ) );
	
	static const float BUDDY_FORCE_MUL = 100.0f;
	static const float ENEMY_FORCE_MUL = 100.0f;
	
	// push towards shell
	velocity += (remainder>0.0f?1.0f:-1.0f)*remainder*remainder*deltaNorm*ofGetLastFrameTime()*BUDDY_FORCE_MUL;

	// run away from enemy
	ofVec2f enemyDelta = neighbours->at( enemy )->position - position;
	float enemyDistance = enemyDelta.length();
	ofVec2f enemyDeltaNorm = enemyDelta/enemyDistance;
	velocity -= enemyDeltaNorm*(1.0f/enemyDistance)*ofGetLastFrameTime()*ENEMY_FORCE_MUL;
}


void FloatingSine::draw()
{
	float vol = volume.getVolume();
	float volSq = vol*vol;
	ofCircle( position.x, position.y, 10.0f );
}
