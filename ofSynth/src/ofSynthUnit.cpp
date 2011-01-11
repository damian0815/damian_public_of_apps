/*
 *  ofSynthUnit.cpp
 *  ofSynth
 *
 *  Created by damian on 10/01/11.
 *  Copyright 2011 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "ofSynthUnit.h"
#include "ofSynthMixer.h"

ofSynthUnit::~ofSynthUnit()
{
	/// tell the mixer we've gone away
	ofSynthMixer::getInstance()->removeUnit( this );
}


/// add an output to the target object
void ofSynthUnit::addOutputTo( ofSynthUnit* target )
{
	ofSynthMixer::getInstance()->addConnection( this, target );
}

/// add an output to the global mixer
void ofSynthUnit::addOutputToMixer()
{
	ofSynthMixer::getInstance()->addMixerInput( this );
}


