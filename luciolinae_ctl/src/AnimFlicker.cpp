/*
 *  AnimFlicker.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 13/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "AnimFlicker.h"

static unsigned int FIRST = 0;
static unsigned int COUNT = 5*16;

const char* AnimFlicker::NAME = "Flicker";

void AnimFlicker::update( float elapsed )
{
	lights->pulse( ofRandom(FIRST,(FIRST+COUNT)-0.00001f ), ofRandomuf(), 
				  /*ofRandom( 1.0f/256, 16.0f/256 )*/0 );
}
