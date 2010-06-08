/*
 *  StateAnimSeq.h
 *  luciolinae_ctl
 *
 *  Created by damian on 08/06/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "StateAnimation.h"

class StateAnimSeq : public StateAnimation
{
public:
	StateAnimSeq( Lights* _lights ) : StateAnimation( _lights ) { };
	
	static const char* NAME;
	string getName() { return NAME; }
	
	void update( float elapsed );
	void draw() {};
	
	void enter();
	bool isFinished() { return count_left < 0; }
	
private:
	
	float anim_pct;
	int current_active;
	float speed;
	int count_left;
};

