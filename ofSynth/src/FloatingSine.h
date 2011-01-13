/*
 *  FloatingSine.h
 *  ofSynth
 *
 *  Created by damian on 12/01/11.
 *  Copyright 2011 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"

class FloatingSine
{
public:
	void setup( ofSoundMixer* mixer, vector<FloatingSine*> * neighbours );
	
	void update();
	
	void draw();
	
	float getFrequency() { return frequency; }
	
	ofVec2f position;
	ofVec2f velocity;

private:
	
	ofSoundSourceTestTone tone;
	ofSoundEffectVolume volume;
	
	vector<FloatingSine*> *neighbours;
	
	float frequency;
	int buddy;
	int enemy;
	float distanceUnits;
	
	float shellDistance;
	
	int octaveOffset;
};
