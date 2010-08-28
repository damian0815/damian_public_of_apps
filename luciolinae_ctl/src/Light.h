/*
 *  Light.h
 *  luciolinae_ctl
 *
 *  Created by damian on 19/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"

class Light
{
public:	
	Light();
	
	void setup( int board_id, int light_id, float _x, float _y ) ;
	void setup( ofxXmlSettings& data );
	void save( ofxXmlSettings& data );
	
	void update( float elapsed );
	void draw();
	
	void pulse( float max_bright, float end_brightness );
	void set( float bright );
	
	// serial
	bool needsSerial() const;
	void resetNeedsSerial();
	
	// brightness + decay
	float getBrightness() const { return brightness; }
	float getPulseDecayFactor() const { return decay_factor; }
	
	// position
	float getX() const { return x; }
	float getY() const { return y; }
	float getZ() const { return z; }
	void setPosition( float _x, float _y ) { x = _x; y = _y; }
	void toggleBig() { big = !big; }
	bool isBig() const { return big; }
	
	// board/light data
	unsigned char getBoardId() const { return board_id; }
	unsigned char getLightId() const { return light_id; }
	
private:
	
	unsigned char board_id, light_id;
	
	float brightness; // 0..1
	float target_brightness;
	
	float decay_factor;
	float last_brightness;
	int needs_count;
	
	bool big;
	float last_set_timer;
	
	float x,y,z;
	
};

