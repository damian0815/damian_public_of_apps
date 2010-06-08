/*
 *  DelaunayPulse.h
 *  luciolinae_ctl
 *
 *  Created by damian on 20/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once
class Lights;

#include <assert.h>

class DelaunayPulse
{
public:
	
	virtual void start( int target, float brightness, float falloff, float speed ) {};
	virtual void update( float elapsed ) {};
	virtual bool isFinished() { return true; }
	
protected:
};


class Lights;
#include <vector>
using namespace std;

class DelaunayPulses
{
public:
	DelaunayPulses( Lights* _lights ) : lights( _lights ) { instance = this; };
	static DelaunayPulses* getInstance() { assert( instance != NULL ); return instance; }
	
	void addPulseIn( int target, float max_brightness=0.2f, float start_radius=0.66f, float speed = 1.0f );
	void addPulseOut( int source, float brightness=0.2f, float falloff = 3.0f, float speed = 1.0f );
	void update( float elapsed );

private:

	Lights* lights;
	
	vector<DelaunayPulse*> pulses;
	
	static DelaunayPulses* instance;

};
