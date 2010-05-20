/*
 *  DelaunayPulseOut.h
 *  luciolinae_ctl
 *
 *  Created by damian on 20/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

class Lights;

#include <set>
#include <queue>
using namespace std;

class DelaunayPulseOut
{
public:
	DelaunayPulseOut( Lights* _lights );
	
	void start ( int which_source, float brightness, float _falloff = 1, float _speed = 1 );
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
	
	// list of currently active
	set<int> seen;

	
	Lights* lights;
	float speed;
	// 1 = normal speed, > = faster, < = slower
	float falloff;
	float timer;
	
	
};
