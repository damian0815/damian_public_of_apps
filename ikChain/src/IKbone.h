/*
 *  IKBone.h
 *  emptyExample
 *
 *  Created by damian on 18/06/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

//#include "ofxVectorMath.h"

class IKBone
{
public:
	IKBone() { length = 1; angle = 0; weight_centre = 0.5f; };

	// weight_centre determines which end of the bone moves:
	// 0   = the start (==parent link) only 
	// 1   = the end   (==child link) only
	// 0.5 = start and end both move the same amount
	IKBone( float _length, float _angle, float _weight_centre )
		: length(_length), angle(_angle), weight_centre(_weight_centre)
		{};

	float getAngle() const { return angle; }
	float getLength() const { return length; }
	float getWeightCentre() const { return weight_centre; }
	void setAngle( float a ) { angle = a; }
	void setLength( float l ) { length = l; }
	void setWeightCentre( float wc ) { weight_centre = wc; }
	
private:
	
	
	float length, weight_centre;
	float angle;

};
