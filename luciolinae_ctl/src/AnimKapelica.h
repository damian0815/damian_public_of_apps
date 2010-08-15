/*
 *  AnimKapelica.h
 *  luciolinae_ctl
 *
 *  Created by damian on 15/08/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "Animation.h"


class KapelicaUnit
{
public:
	
	typedef enum { KU_IDLE, KU_DELAUNAY_DOWN, KU_PULSE, KU_DELAUNAY_UP } KUState;
	
	KapelicaUnit();
	KapelicaUnit( int _id );
	
	
	void update( Lights* lights, float elapsed );
	
private:
	
	int id;
	float timer;
	float time_scalar;
	
	KUState state;
	
};


class AnimKapelica: public Animation
{
public:
	AnimKapelica( Lights* _lights );
	
	static const char* NAME;
	
	void update( float elapsed );
	void draw() {};
	
	
protected:
	
	vector<KapelicaUnit> units;
	
	float timer;
};

