/*
 *  LightsDelaunay.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 18/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "LightsDelaunay.h"
#include "ofxVectorMath.h"


struct IndexedVec3f
{
	int index;
	ofxVec3f vec;
};

static int IndexedVec3fCompare( const void *v1, const void *v2) 
{
	IndexedVec3f *p1, *p2;
	
	p1 = (IndexedVec3f*)v1;
	p2 = (IndexedVec3f*)v2;
	if(p1->vec.x < p2->vec.x)
		return(-1);
	else if(p1->vec.x > p2->vec.x)
		return(1);
	else
		return(0);
	
}

LightsDelaunay::LightsDelaunay( const Lights& lights )
{
	valid_lights.clear();
	
	// first, sort vertices, maintaining index associations
	
	int num_lights = lights.getNumLights();
	IndexedVec3f vertices[num_lights];
	for ( size_t i=0; i<num_lights; i++ )
	{
		valid_lights.push_back( i) ;
		vertices[i].index = i;
		vertices[i].vec = ofxVec3f( lights.getLight(i).getX(), lights.getLight(i).getY(), lights.getLight(i).getZ() );
	}
	qsort(vertices,num_lights,sizeof(IndexedVec3f),IndexedVec3fCompare);
	// copy sort output to a new array
	Delaunay::XYZ xyz[num_lights+3];
	for ( size_t i=0; i<num_lights; i++ )
	{
		xyz[i].x = vertices[i].vec.x;
		xyz[i].y = vertices[i].vec.y;
		xyz[i].z = vertices[i].vec.z;
	}
	// prepare output storage
	Delaunay::ITRIANGLE triangles[3*num_lights];
	int num_triangles;
	Delaunay::Triangulate( num_lights, xyz, triangles, num_triangles );
	
	// construct adjacency lists
	adjacencies.resize( num_lights );
	for ( int i=0; i<num_triangles; i++ )
	{
		int a = vertices[triangles[i].p1].index;
		int b = vertices[triangles[i].p2].index;
		int c = vertices[triangles[i].p3].index;
		
		// connect point a to points b + c
		adjacencies[a].insert( b );
		adjacencies[a].insert( c );
		
		// connect point b to points a + c
		adjacencies[b].insert( a );
		adjacencies[b].insert( c );
		
		// connect point c to points a + b
		adjacencies[c].insert( a );
		adjacencies[c].insert( b );
	}
	
}

LightsDelaunay::LightsDelaunay( const Lights& lights, int use_board_id )
{
	valid_lights.clear();

	// first, sort vertices, maintaining index associations
	vector<int> board_lights = lights.getLightIdsForBoard( use_board_id );
	int num_lights = board_lights.size();
	IndexedVec3f vertices[num_lights];
	for ( size_t i=0; i<num_lights; i++ )
	{
		valid_lights.push_back( board_lights[i] ) ;
		vertices[i].index = board_lights[i];
		vertices[i].vec = ofxVec3f( lights.getLight(board_lights[i]).getX(), lights.getLight(board_lights[i]).getY(), 0 );
	}
	qsort(vertices,num_lights,sizeof(IndexedVec3f),IndexedVec3fCompare);
	// copy sort output to a new array
	Delaunay::XYZ xyz[num_lights+3];
	for ( size_t i=0; i<num_lights; i++ )
	{
		xyz[i].x = vertices[i].vec.x;
		xyz[i].y = vertices[i].vec.y;
		xyz[i].z = vertices[i].vec.z;
	}
	// prepare output storage
	Delaunay::ITRIANGLE triangles[3*num_lights];
	int num_triangles;
	Delaunay::Triangulate( num_lights, xyz, triangles, num_triangles );
	
	// construct adjacency lists
	adjacencies.resize( lights.getNumLights() );
	for ( int i=0; i<num_triangles; i++ )
	{
		int a = vertices[triangles[i].p1].index;
		int b = vertices[triangles[i].p2].index;
		int c = vertices[triangles[i].p3].index;
		
		// connect point a to points b + c
		adjacencies[a].insert( b );
		adjacencies[a].insert( c );
		
		// connect point b to points a + c
		adjacencies[b].insert( a );
		adjacencies[b].insert( c );
		
		// connect point c to points a + b
		adjacencies[c].insert( a );
		adjacencies[c].insert( b );
	}
	
}

