/*
 *  NearestPointToLine3D.h
 *  luciolinae_ctl
 *
 *  Created by damian on 13/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */


#include "ofxVectorMath.h"


namespace NearestPointOnLineToPoint
{
	extern ofxVec3f calculateNearestPoint( const ofxVec3f &LineStart, const ofxVec3f &LineEnd, const ofxVec3f &Point, 
										  float* linePosition=NULL );

};
