/*
 *  StateAnimDelaunayOut.h
 *  luciolinae_ctl
 *
 *  Created by damian on 20/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "StateAnimation.h"
#include "DelaunayPulseOut.h"

class StateAnimDelaunayOut : public StateAnimation
{
public:
	StateAnimDelaunayOut( Lights* _lights ) : StateAnimation( _lights ), delaunay_out( _lights ) {};
	
	static const char* NAME;
	string getName() { return NAME; }
	
	void update( float elapsed ) { delaunay_out.update( elapsed ); }
	void draw() {};
	
	void enter() { 
		// blip the appropriate light
		int from_which = SharedData::getFloat( "blip_target" );
		delaunay_out.start( from_which, 0.1f );
	}
	bool isFinished() { return delaunay_out.isFinished(); }
	
private:
	
	DelaunayPulseOut delaunay_out;
	
};
