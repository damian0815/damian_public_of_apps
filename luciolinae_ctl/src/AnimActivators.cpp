/*
 *  AnimActivators.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 18/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "AnimActivators.h"

static const int NUM_ACTIVATORS =3;

const char* AnimActivators::NAME = "Activators";

AnimActivators::AnimActivators( Lights* _lights )
:Animation (_lights)
{
	activators.setup( NUM_ACTIVATORS, lights );
}

void AnimActivators::update( float elapsed )
{
	activators.update( elapsed );
}

void AnimActivators::draw()
{
	activators.draw();
}
