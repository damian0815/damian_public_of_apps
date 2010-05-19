/*
 *  AnimActivators.h
 *  luciolinae_ctl
 *
 *  Created by damian on 18/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "Animation.h"
#include "Activators.h"

class AnimActivators: public Animation
{
public:
	AnimActivators( Lights* _lights );
	
	static const char* NAME;
	
	void update( float elapsed );
	void draw();
	
	
private:

	Activators activators;
	

	

};