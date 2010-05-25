/*
 *  StateAnimBlip.h
 *  luciolinae_ctl
 *
 *  Created by damian on 19/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once
#include "Util.h"
#include "Osc.h"

#include "StateAnimation.h"

class StateAnimBlip : public StateAnimation
{
public:
	StateAnimBlip( Lights* _lights ) : StateAnimation( _lights ) {};
	
	static const char* NAME;
	string getName() { return NAME; }
	
	void update( float elapsed ) { timer -= elapsed; }
	void draw() {};
	
	void enter() { 
		// blip the appropriate light
		int which = SharedData::getFloat( "blip_target" );
		float blip_count = SharedData::getFloat( "blip_count" );
		lights->pulse( which, 1, true );
		timer = squaredRandom( 0.3f, 0.7f ) + blip_count*blip_count*0.2f; 
		
		ofxOscMessage m;
		m.setAddress("/blip/trigger");
		m.addFloatArg( blip_count );
		Osc::sendMessage( m );
		SharedData::setFloat( "blip_count", blip_count+1 );
	}
	bool isFinished() { return timer < 0; }
	
private:
	
	float timer;
	
};
