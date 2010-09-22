/*
 *  AnimGazebo.h
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
#include "LightsDelaunay.h"

class AnimGazebo: public Animation
{
public:
	AnimGazebo( Lights* _lights ) ;
	
	static const char* NAME;
	
	void update( float elapsed );
	void draw();
	
	void keyPressed( int k );
	void mouseMoved( int x, int y );
	
private:
	// trigger a delaunay pulse starting at index
	void triggerDelaunay ( int index );
	// delete the edge between closest_a and closest_b
	void deleteCurrentEdge();
	// undo the last edge deletion
	void undoDeleteEdge();
	// save
	void saveDelaunay();
	
	LightsDelaunay customDelaunay;
	
	int which;
	bool editing_delaunay;
	
	float speed;
	int closest_a, closest_b;
	float timer;
	
	vector<pair<int, int> > removed_neighbours;
	bool inout;
};