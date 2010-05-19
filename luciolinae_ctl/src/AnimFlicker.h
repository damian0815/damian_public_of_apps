/*
 *  AnimFlicker.h
 *  luciolinae_ctl
 *
 *  Created by damian on 13/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "Animation.h"

class AnimFlicker: public Animation
{
public:
	AnimFlicker( Lights* _lights ) :	Animation( _lights ) {};
	
	static const char* NAME;
	
	void update( float elapsed );
	void draw() {};
	
protected:
	
};

