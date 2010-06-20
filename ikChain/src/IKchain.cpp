/*
 *  IKchain.cpp
 *  emptyExample
 *
 *  Created by damian on 18/06/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "IKchain.h"

void IKChain::addBone( float length, float angle, float weight_centre )
{
	bones.push_back( IKBone( length, angle, weight_centre ) );
}

