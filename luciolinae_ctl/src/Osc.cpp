/*
 *  Osc.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 20/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "Osc.h"

Osc* Osc::instance = NULL;

void Osc::setup( string host, int port )
{
	send.setup( host, port );
}

