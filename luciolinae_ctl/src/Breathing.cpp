/*
 *  Breathing.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 20/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "Breathing.h"
#include "Osc.h"
#include "SharedData.h"
#include "Util.h"



// inhale is slower than exhale
// hold is shorter than wait
// inhale is fast for first 75% -> right-skewed sine
// exhale is fast for first 75% -> right-skewed sine
// 'right skewed sine': rss(x) for x 0..1 = x^-1.5
//    -> velocity is -1.5*x^(-2.5)


float BREATH_TIME = 60.0f/12.0f;

void Breathing::update( float elapsed )
{
	timer += elapsed;
	while ( timer > breath_time )
	{
		// reset timer 
		timer -= breath_time;
		breath_time = BREATH_TIME* squaredRandom( 1.0f, 1.3f );
	}
	
	float speed;
	float pct;
	if ( (timer/breath_time) < 0.5f )
	{
		pct = (timer/breath_time)/0.5f;
		lung_fullness = 1-(1-pct)*(1-pct);
		// = 1-(1- 2*x + x^2)
		// = 2*x - x^2;
		// integrate:
		// = x^2 - (1/3)*(x^3) + c
		//lung_fullness = pct*pct - (0.33333f*pct*pct*pct);
		speed = 2-2*pct;
	}
	else
	{
		pct = ((timer/breath_time)-0.5f)/0.5f;
		lung_fullness = (1-pct)*(1-pct);
		// = 1 - 2*x + x^2
		// integrate
		// = x - x^2 + (1/3)*(x^3) + c
		//lung_fullness = pct - (pct*pct) + (0.33333f*pct*pct*pct);
		speed = -2+2*pct;
		
	}

	/*
	float target_speed;
	if ( timer<0.5f )
	{
		// work out a speed to fill our lungs by t=0.5
		target_speed = (1-lung_fullness)/(0.5f-timer);
	}
	else
	{
		// work out a speed to empty our lungs by t=1
		target_speed = -lung_fullness/(1-timer);
	}
	speed = speed*(1-elapsed) + target_speed*elapsed;
	lung_fullness += speed*elapsed;*/
	
	ofxOscMessage m;
	m.setAddress( "/breath" );
	if ( speed > 0 )
		m.addFloatArg( 1 );
	else
		m.addFloatArg( 0 );
	m.addFloatArg( lung_fullness );
	speed_smoothed = speed_smoothed * 0.75f + speed * 0.25f;
	m.addFloatArg( speed_smoothed );
	m.addFloatArg( speed*elapsed );
	Osc::getInstance()->sendMessage( m );
	
	SharedData::setFloat("breath", lung_fullness );
	
	/*
	int full_bar_length = lung_fullness*20;
	printf("breath %6.3f %6.3f %s %7.4f |", pct, speed, speed>0?"in ":"out", lung_fullness );
	for ( int i=0; i<full_bar_length; i++ )
	{
		printf("*");
	}
	printf("|\n");
	 */
}



