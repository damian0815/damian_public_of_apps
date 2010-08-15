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
	KapelicaUnit( int _id ) { id = _id; cycle_time = ofRandom( 2.0f, 3.0f ); timer = cycle_time; state = KU_IDLE; };
	
	
	void update( Lights* lights, float elapsed )
	{
		timer -= elapsed;
		if ( state == KU_IDLE && timer < 0.0f )
		{
			state = KU_DELAUNAY_DOWN;
			

			// pulse the big light
			int which = lights->getBigLightIndex( id );
			
			lights->pulse( which, 1.0f, true );
			timer += cycle_time;
		}
	}
	
private:
	
	int id;
	float timer;
	float cycle_time;
	
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

