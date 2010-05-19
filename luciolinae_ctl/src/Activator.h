/*
 *  Activator.h
 *  serialExample
 *
 *  Created by damian on 12/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

class Activator
{
public:
	Activator() {};
	
	/// position the activator here (x/y 0..1) with the given power
	void setup(float ix, float iy, float _power );
	
	/// move the activator to the given pos at the given speed, drawing a line between
	void moveTo( float nx, float ny, float _speed );
	/// move the activator to the given pos immediately
	void moveTo( float nx, float ny );
	
	/// update activator position
	void update( float elapsed );
	
	/// draw
	void draw();
	
	/// return an activation energy (0..1) for the given light position
	float getActivationEnergy( float lx, float ly );

private:
	
	// old pos
	float ox, oy; 
	// current pos
	float x, y;
	// energy; decides how far out the energy will reach
	float power;
	
	float tx, ty;
	float speed;
	



};
