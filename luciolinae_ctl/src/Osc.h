/*
 *  Osc.h
 *  luciolinae_ctl
 *
 *  Created by damian on 20/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxOsc.h"

class Osc
{
public:	
	Osc() { instance = this; }
	static Osc* getInstance() { return instance; }
	
	// communicate with given host and port
	void setup( string host, int port );
	
	// send a message
	static void sendMessage( ofxOscMessage& m ) { instance->send.sendMessage( m ); }
	
private:
	
	static Osc* instance;
	
	ofxOscReceiver rec;
	ofxOscSender send;
};
