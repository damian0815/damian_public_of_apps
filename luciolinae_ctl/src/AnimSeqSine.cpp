/*
 *  AnimSeqSine.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 18/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "AnimSeqSine.h"

const char* AnimSeqSine::NAME = "SeqSine";

static const float BRIGHT=sqrtf(1.0f);

AnimSeqSine::AnimSeqSine( Lights* _lights ) 
:	Animation( _lights )
{
	speed = 0.2f;
	phases.resize( lights->getNumLights() );
	for ( int i=0; i<lights->getNumLights(); i++ )
	{
		phases[i] = float(i)/lights->getNumLights();
	}
}

void AnimSeqSine::update( float elapsed )
{
	
	for ( int i=0; i<phases.size(); i++ )
	{
		phases[i] += elapsed*speed;
		float bright = BRIGHT*(sinf(TWO_PI*phases[i])*0.5f+0.5f);
		bright*=bright;
		lights->set( i, bright, true );
	}
	
}
