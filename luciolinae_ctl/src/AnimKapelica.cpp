/*
 *  AnimKapelica.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 15/08/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "AnimKapelica.h"

const char* AnimKapelica::NAME = "Kapelica";



AnimKapelica::AnimKapelica( Lights* _lights ) 
:	Animation( _lights )
{
	for ( int i=0; i<4; i++ )
	{
		units.push_back( KapelicaUnit( i ) );
	}
}

void AnimKapelica::update( float elapsed )
{
	for ( int i=0; i<units.size(); i++ )
	{
		units[i].update( lights, elapsed );
	}
}

