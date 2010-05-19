/*
 *  AnimRotate.h
 *  serialExample
 *
 *  Created by damian on 07/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "Animation.h"

class AnimRotate: public Animation
{
public:
	AnimRotate( Lights* _lights );
	
	void update( float elapsed );
	void draw() {};
	
	
protected:
	
	// 0..1
	float anim_pct;
	
	// pct/second
	float speed;

	// current light
	int current_active;
};

