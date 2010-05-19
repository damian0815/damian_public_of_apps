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

class AnimDelaunay: public Animation
{
public:
	AnimDelaunay( Lights* _lights ) : Animation( _lights ),timer(0) {};

	static const char* NAME;
	
	void update( float elapsed );
	void draw();
	
	void keyPressed( int k );
	
private:
	// trigger a delaunay pulse starting at index
	void triggerDelaunay ( int index );

	// list of currently active
	set<int> seen;

	class DelaunayPulse
	{
	public:
		DelaunayPulse( int _id, float _energy, float _timer ) { id = _id; energy = _energy; timer = _timer; }
		bool operator<( const DelaunayPulse& other ) const { return timer > other.timer; }
		int id;
		float timer;
		float energy;
	};
	typedef priority_queue<DelaunayPulse> DelaunayPulses;
	DelaunayPulses queued_pulses;
	
	float timer;


};