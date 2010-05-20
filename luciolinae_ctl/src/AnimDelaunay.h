/*
 *  AnimDelaunay.h
 *  luciolinae_ctl
 *
 *  Created by damian on 18/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once
#include "Animation.h"
#include <set>
#include <queue>
#include "DelaunayPulseOut.h"

class AnimDelaunay: public Animation
{
public:
	AnimDelaunay( Lights* _lights ) : Animation( _lights ) { which = ofRandom( 0, lights->getNumLights()*0.999f );};

	static const char* NAME;
	
	void update( float elapsed );
	void draw();
	
	void keyPressed( int k );
	
private:
	// trigger a delaunay pulse starting at index
	void triggerDelaunay ( int index );
	
	int which;

};