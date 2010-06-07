/*
 *  StateAnimBlipAll.h
 *  luciolinae_ctl
 *
 *  Created by damian on 05/06/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once


#pragma once
#include "Util.h"
#include "Osc.h"

#include "StateAnimation.h"

class StateAnimBlipAll: public StateAnimation
{
public:
	StateAnimBlipAll( Lights* _lights ) : StateAnimation( _lights ) {};
	
	static const char* NAME;
	string getName() { return NAME; }
	
	void update( float elapsed );
	void draw() {};
	
	void enter() { 
		blipped = false;
		timer = squaredRandom( 0.3f, 0.7f ); 
	}
	bool isFinished() { return blipped && timer < 0; }
	
private:

	bool blipped;
	float timer;
	

};