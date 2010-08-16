/*
 *  LightsDelaunay.h
 *  luciolinae_ctl
 *
 *  Created by damian on 18/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "Lights.h"
#include "Delaunay.h"
#include <set>

class LightsDelaunay
{
public:
	/// construct Delaunay triangulation for lights
	LightsDelaunay( const Lights& lights );
	LightsDelaunay( const Lights& lights, int use_board_id );

	/// return the ids of all the neighbours of this light
	const set<int> getNeighbours( int target_id ) { return adjacencies.at(target_id); }

private:

	// adjacency lists
	vector<set<int> > adjacencies;

};

