/*
 *  Lights.h
 *  serialExample
 *
 *  Created by damian on 07/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "BufferedSerial.h"
#include <set>
#include "Light.h"
class LightsDelaunay;


class Lights
{
public:
	Lights() { rebuild_delaunay = true; delaunay = NULL; }
	~Lights();
	
	void setup( BufferedSerial* serial );
	void setup( BufferedSerial* serial, ofxXmlSettings &data );
	void save( ofxXmlSettings& data );

	void update( float elapsed );

	void draw();

	// all brightness settings are 0..1
	void pulse( int id, float max_bright, float decay_factor= 0.3f/* 0.1 = very fast, 0.9 = very slow */  );
	void pulseAll( float max_bright, float decay_factor= 0.3f /* 0.1 = very fast, 0.9 = very slow */  );
	void set( int id, float bright );

	// all off; if pummel is true then do this very thoroughly
	void clear( bool pummel=false );
	
	// illuminate in this area
	void illuminateCircularArea( float x, float y, float area );
	// illuminate a line of area. (x,y) is a point on the line, (dx,dy) is a direction vector
	// and width is the illumination width of the line
	void illuminateCorridor( float x, float y, float dx, float dy, float power, float width );
	// only draw
	void drawIlluminateCorridor( float x, float y, float dx, float dy, float power, float width );
	
	// send all brightness commands immediately and latch in
	void flush();
	
	// accessors
	int getNumLights() const { return lights.size(); }
	const Light& getLight( int i ) const { return lights.at(i); }
	void setLightPosition( int id, float x, float y ) { lights.at(id).setPosition( x, y ); rebuild_delaunay = true; }
	
	int getNumBigLights() const { return big_lights.size(); }
	const Light& getBigLight( int i ) const { return lights[big_lights.at(i)]; }
	int getBigLightIndex( int i ) const { return big_lights.at(i); }
	void toggleLightIsBig( int id );
	
	// get delaunay triangulation
	LightsDelaunay* getDelaunay();
	
	

private:
	// takes brightness of 0..4095
	void sendLightLevel( unsigned char board_id, unsigned char light_id, int brightness );
	// takes brightness of 0..4095
	// decay is in steps (1/4096'th) per millisecond
	void sendPulse( unsigned char board_id, unsigned char light_id, int brightness, unsigned char decay );
	// send packed data
	void sendEveryLightLevel( unsigned char board_id, unsigned char data[24] );
	// latch in
	void latch( unsigned char board_id=0 /* default all */ );
	
	BufferedSerial* serial;
	LightsDelaunay* delaunay;
	
	int num_boards;
	bool rebuild_delaunay;
	
	
	vector<Light> lights;

	vector<int> big_lights;
	

};

