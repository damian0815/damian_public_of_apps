/*
 *  DelaunayPulseIn.h
 *  luciolinae_ctl
 *
 *  Created by damian on 20/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */


#pragma once

#include "DelaunayPulse.h"

#include <set>
#include <queue>
using namespace std;

class DelaunayPulseIn : public DelaunayPulse
{
public:
	DelaunayPulseIn( Lights* _lights ) : lights( _lights ) {};
	
	// buildup is the inverse of falloff
	void start ( int which_source, float start_radius, float max_brightness, float _speed );
	void update( float elapsed );
	bool isFinished() { return queued_pulses.empty(); }
	
private:
	
	class MovingPulse
	{
	public:
		MovingPulse( int _id, float _energy, float _timer ) { id = _id; energy = _energy; timer = _timer; }
		bool operator<( const MovingPulse& other ) const { return timer > other.timer; }
		int id;
		float timer;
		float energy;
	};
	typedef priority_queue<MovingPulse> MovingPulses;
	MovingPulses queued_pulses;
	
	
	int target_index;
	float speed;
	// 1 = normal speed, > = faster, < = slower
	float initial_radius;
	float max_brightness;
	float timer;
	
	Lights* lights;
	
};
