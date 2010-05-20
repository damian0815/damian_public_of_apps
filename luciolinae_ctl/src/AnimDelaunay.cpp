/*
 *  AnimDelaunay.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 18/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "AnimDelaunay.h"
#include "LightsDelaunay.h"
#include "ofxVec2f.h"

static const float STEP_TIME = 0.08f;

const char* AnimDelaunay::NAME = "Delaunay";


void AnimDelaunay::update( float elapsed )
{
	delaunay_pulse.update( elapsed );
}

void AnimDelaunay::keyPressed ( int k ) 
{
	if ( k == OF_KEY_RETURN )
	{
		triggerDelaunay( ofRandom( 0, lights->getNumLights()*0.999f ) );
	}
}


void AnimDelaunay::triggerDelaunay( int index )
{
	index = min(lights->getNumLights()-1,max(0,index));
	delaunay_pulse.start( index, 0.1f, 1, 1);
}


void AnimDelaunay::draw()
{
	ofSetColor( 0x20, 0x80, 0x20, 0x10 );
	for ( int i=0; i<lights->getNumLights(); i++ )
	{
		set<int> adj = lights->getDelaunay()->getNeighbours( i );
		for ( set<int>::iterator jt = adj.begin();
			 jt != adj.end();
			 ++jt )
		{
			ofLine( ofGetWidth()*lights->getLight( i ).getX(),   ofGetHeight()*lights->getLight( i ).getY(),
				    ofGetWidth()*lights->getLight( *jt ).getX(), ofGetHeight()*lights->getLight( *jt ).getY() );
		}
	}
}
