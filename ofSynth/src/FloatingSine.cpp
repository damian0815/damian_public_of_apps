/*
 *  FloatingSine.cpp
 *  ofSynth
 *
 *  Created by damian on 12/01/11.
 *  Copyright 2011 frey damian@frey.co.nz. All rights reserved.
 *
 */


#include "FloatingSine.h"


static const float RADIUS = 10.0f;

/*
// pentatonic
static const int	SCALE_STEPS = 5;
static const float	SCALE[SCALE_STEPS] = { 0, 2, 5, 7, 10 };
 */


// lydian Tone, tone, semitone, tone, tone, tone, semitone
static const int	SCALE_STEPS = 8;
static const float	SCALE[SCALE_STEPS] = { 0, 2, 3, 5, 7, 9, 10 };


static const float BASE_MIDI_NOTE = 62.0f;


float midiToFrequency( float midiNote )
{
	return 440.0f*powf(2.0f,(midiNote-60.0f)/12.0f);
}

void FloatingSine::setup( ofSoundMixer* mixer, vector<FloatingSine*> * n )
{
	neighbours = n;

//	const float E = 329.6f;
//	float baseFrequency = E*powf(2.0f,int(ofRandom( -2, 2 )));
	int whichPentatonic = ofRandom( 0, 0.99999f*SCALE_STEPS );
	float midiNote = BASE_MIDI_NOTE + SCALE[whichPentatonic];
	float baseFrequency = midiToFrequency( midiNote );
	ofLog(OF_LOG_NOTICE, "tone %x: freq %f", &tone, baseFrequency );
	

	shellDistance = ofRandom( 100.0f, 200.0f );
	
	frequency = baseFrequency;
	tone.setFrequency( frequency );
/*	if ( ofRandomuf() > 0.5f ) */
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
	
	octaveOffset = ofRandom( -2.99f, 0.99f );
}



void FloatingSine::update( )
{
	// update the position
	position += velocity*ofGetLastFrameTime();
	
	// damp the velocity
	velocity *= powf(0.73f,ofGetLastFrameTime());


	ofVec2f delta = neighbours->at( buddy )->position - position;
	float distance = delta.length();
	ofVec2f deltaNorm = delta/distance;
	
	distanceUnits = distance/shellDistance;
	
	// calculate distance in terms of shells
	int whichPentatonic = distanceUnits;
	float remainder = distanceUnits - whichPentatonic;
	if ( remainder > 0.5f )
	{
		distanceUnits += 1.0f;
		whichPentatonic += 1.0f;
		remainder = 1.0f-remainder;
	}

	// update frequency
	float midiNote = BASE_MIDI_NOTE;
	while ( whichPentatonic>=SCALE_STEPS )
	{
		midiNote += 12; 
		whichPentatonic -= SCALE_STEPS;
	}
	
	float scaleNote = SCALE[whichPentatonic%SCALE_STEPS];
	float nextScaleNote = SCALE[(whichPentatonic+1)%SCALE_STEPS];
	float prevScaleNote = SCALE[(whichPentatonic+4)%SCALE_STEPS];
	if ( prevScaleNote>nextScaleNote) 
	{
		nextScaleNote += 12.0f;
	} 
	
/*	midiNote += (nextScaleNote-prevScaleNote)*sinf(remainder*2.0f*TWO_PI);
	tone.setFrequency( midiToFrequency( midiNote ) );*/
	tone.setFrequency( midiToFrequency( float(octaveOffset*12) + midiNote + scaleNote + remainder*0.1f /* (remainder>0.0f?1.0f:-1.0f)*remainder*remainder*/ ) ); 
	
	// volume
	float vol = 1.0f-min(1.0f,(distance/ofGetWidth()));
	volume.setVolume( vol/neighbours->size() );
	
	
	static const float BUDDY_FORCE_MUL = 1000.0f;
	static const float ENEMY_FORCE_MUL = 100.0f;
	static const float CENTRE_FORCE_MUL = 0.3f;
	

	// push towards shell
	velocity += ((remainder>0.0f)&&(distanceUnits>1.0f)>0.0f?-1.0f:1.0f)*
		remainder*remainder*deltaNorm*ofGetLastFrameTime()*BUDDY_FORCE_MUL;
	
	
	/*
	for ( int i=0; i<neighbours->size(); i++ )
	{
		for ( int j=i+1; j<neighbours->size(); j++ )
		{
			ofVec2f neighbourPos = neighbours->at(i)->position;
			if ( (neighbourPos-position).lengthSquared() < 4.0f*RADIUS*RADIUS )
			{
				// reflect
				ofVec2f delta = neighbourPos-position;
				ofVec2f deltaNorm = delta.normalized();

				// move out
				ofVec2f shouldBeAt = neighbourPos - deltaNorm*RADIUS*2;
				ofVec2f move = shouldBeAt-position;
				position += move;
				neighbours->at(i)->position -= move;
				
				ofVec2f velocityParallel = velocity.dot( deltaNorm );
				ofVec2f velocityAdj = velocity - velocityParallel;
				//velocity = velocityAdj - velocityParallel*0.5f;
				//neighbours->at(i)->velocity = 0.5f*(deltaAdj-deltaParallel);
			}
		}
	}
	 */
	
	
	// run away from enemy
	ofVec2f enemyDelta = neighbours->at( enemy )->position - position;
	float enemyDistance = enemyDelta.length();
	ofVec2f enemyDeltaNorm = enemyDelta/enemyDistance;
	velocity -= enemyDeltaNorm*(1.0f/enemyDistance)*ofGetLastFrameTime()*ENEMY_FORCE_MUL;
	
	// centre tug
	ofVec2f centreDelta = ofVec2f(ofGetWidth()/2,ofGetHeight()/2)-position;
	velocity += centreDelta*ofGetLastFrameTime()*CENTRE_FORCE_MUL;
}


void FloatingSine::draw()
{
	float vol = volume.getVolume();
	float volSq = vol*vol;
	float alpha = fabsf(2.0f*(distanceUnits-int(distanceUnits)-0.5f));
	ofSetColor( 160, 100, 100, 255 );
	ofFill();
	ofCircle( position.x, position.y, RADIUS );
	
	ofSetColor( 160, 100, 100, 255*alpha );
	ofNoFill();
	ofVec2f buddyPosition = neighbours->at(buddy)->position;
	ofLine( position.x, position.y, buddyPosition.x, buddyPosition.y );

	ofCircle( buddyPosition.x, buddyPosition.y, distanceUnits*shellDistance );
}

