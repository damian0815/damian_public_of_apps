/*
 *  Activator.cpp
 *  serialExample
 *
 *  Created by damian on 12/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */


#include "Activator.h"
#include "ofxVectorMath.h"
#include "NearestPointOnLineToPoint.h"

#include "ofMain.h"

void Activator::setup( float ix, float iy, float _power )
{
	power = _power;
	moveTo( ix, iy );
	//printf("setup: this %lx, old_pos %f %f, current_pos %f %f, target %f %f\n", this, ox, oy, x, y, tx, ty );

}

void Activator::update( float elapsed )
{
	ofxVec2f current_pos(x,y);

	// move activator
	// calculate direction vector
	ofxVec2f delta( tx-x, ty-y );
	// not there?
	if ( delta.length() > 0.001f )
	{
		// normalize delta
		ofxVec2f direction_vector = delta.normalized();
		// try to get there in 1 second
		float distance_to_move = min(speed*elapsed, 1.0f/* don't overshoot */ ) * 
			delta.length();

		// calculate new pos
		ofxVec2f new_pos = current_pos + direction_vector*distance_to_move;

		// update current pos
		ox = x;
		oy = y;
		x = new_pos.x;
		y = new_pos.y;
	//	printf("moved to %f %f (target %f %f, delta %f %f, distance to move %f)\n", x, y, delta.x, delta.y, tx, ty, distance_to_move );
		
	}	
	else
	{
		ox=x;
		oy=y;
	}
}


void Activator::moveTo( float nx, float ny, float _speed )
{
	tx = nx;
	ty = ny;
	speed = _speed;
}

void Activator::moveTo( float nx, float ny )
{
	tx = nx;
	ty = ny;
	x = nx;
	y = nx;
	ox = nx;
	oy = ny;
}

/// return an activation energy (0..1) for the given light position
float Activator::getActivationEnergy( float lx, float ly )
{
	// work out a distance from the motion line
	ofxVec3f old_pos( ox, oy, 0.0f );
	ofxVec3f current_pos( x, y, 0.0f );
	ofxVec3f light_pos( lx, ly, 0.0f );
	//printf("this %lx, old_pos %f %f, current_pos %f %f, light_pos %f %f\n", this, ox, oy, x, y, lx, ly );
	float line_position;
	ofxVec3f nearest_point_on_line;
	bool did_line = false;
	float move_distance = (old_pos-current_pos).length();
	float power_multiplier = 1.0f;
	if ( move_distance>0.00001f )
	{
		nearest_point_on_line = NearestPointOnLineToPoint::calculateNearestPoint(old_pos, current_pos, light_pos, &line_position );
		did_line = true;
		power_multiplier = 1+move_distance;
	}
	else
	{
		nearest_point_on_line = current_pos;
		line_position = 1.0f;
	}
	
	// distance -> brightness
	if ( line_position < 0 )
		nearest_point_on_line = old_pos;
	else if ( line_position > 1 )
		nearest_point_on_line = current_pos;
	// clamp line position to 0..1
	line_position = min(1.0f, max(0.0f, line_position));
	// calculate distance with square falloff
	float distance = (nearest_point_on_line - light_pos).length();
	//printf("distance to light is %f\n", sqrtf(distance_sq) );
		
	// incorporate power, which decides how far out the light will reach
	// if power == 1, then brightness = max(0, 1-normalised_distance);

	// normalise the distance
	float distance_max = power_multiplier*power*power;
	if ( distance <= distance_max )
	{
		float brightness = sqrtf(1.0f-(distance/distance_max));
		//printf("distance %f: power %f -> raw brightness %f\n", distance, power, brightness ); 
			
		// more energy at the end of the line (1), less at the start (0)
		//brightness *= (0.7f + line_position*0.3f);
		/*
		 printf("%s _%7.5f_ distance %f to nearest point %f %f (U %f)\n", did_line?"[line] ":"[point]", 
			   brightness, distance, nearest_point_on_line.x, nearest_point_on_line.y, line_position );
		 */
		// return
		return brightness;
	}
	else
		return 0;
}


void Activator::draw()
{
	// line from x to y
	ofSetColor( 0xff, 0xff, 0xff, 0x05 );
	ofLine( ofGetWidth()*ox, ofGetHeight()*oy, ofGetWidth()*x, ofGetHeight()*y );
	// circle at end point
	ofNoFill();
	ofCircle( ofGetWidth()*x, ofGetHeight()*y, ofGetWidth()*power*power );
			 
}

