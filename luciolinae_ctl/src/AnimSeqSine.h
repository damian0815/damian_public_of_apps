/*
 *  AnimSeqSine.h
 *  luciolinae_ctl
 *
 *  Created by damian on 18/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "Animation.h"

class AnimSeqSine: public Animation
{
public:
	AnimSeqSine( Lights* _lights );
	
	static const char* NAME;
	
	void update( float elapsed );
	void draw() {};
	
	
protected:
	
	// phase pct/second
	float speed;

	// phases
	vector<float> phases;
};

