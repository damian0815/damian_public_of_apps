/*
 *  StateAnimChunk.h
 *  luciolinae_ctl
 *
 *  Created by damian on 25/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "StateAnimation.h"

class StateAnimChunk : public StateAnimation
{
public:
	StateAnimChunk( Lights* _lights ) : StateAnimation( _lights ) {};
	
	static const char* NAME;
	string getName() { return NAME; }
	
	void update( float elapsed );
	void draw() {};
	
	void enter();
	bool isFinished() { return chunks == 0 && timer < 0; }
	
private:
	
	int chunks;
	int total_chunks;
	float chunk_time;
	float timer;
	
};

