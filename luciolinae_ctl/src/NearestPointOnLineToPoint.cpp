/*
 *  NearestPointOnLineToPoint.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 18/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */


#include "NearestPointOnLineToPoint.h"

// stolen from
// http://local.wasp.uwa.edu.au/~pbourke/geometry/pointline/

// Calculate the nearest point on the given line segment to the given point.
// Return the intersection point on the line. If linePosition is non-NULL, 
// set it to be the normalised line position (0=LineStart, 1=LineEnd, <0=before 
// LineStart, >1=after LineEnd).
ofxVec3f NearestPointOnLineToPoint::calculateNearestPoint( const ofxVec3f &LineStart, const ofxVec3f &LineEnd, const ofxVec3f &Point, 
							   float* linePosition, bool clamp_to_line )
{
	float LineMagSq;
	float U;
	
	LineMagSq = (LineEnd-LineStart).lengthSquared();
	if ( LineMagSq > (0.000001f*0.000001f) )
	{
		U = ( 
			 ( ( Point.x - LineStart.x ) * ( LineEnd.x - LineStart.x ) ) +
			 ( ( Point.y - LineStart.y ) * ( LineEnd.y - LineStart.y ) ) +
			 ( ( Point.z - LineStart.z ) * ( LineEnd.z - LineStart.z ) ) 
			 ) / LineMagSq;
	}
	else
		U = 1.0f;
	
	
	if ( clamp_to_line )
		U = max( 0.0f, min(U, 1.0f ) );
	
	ofxVec3f intersection = LineStart + U*(LineEnd-LineStart);
	// save linePosition
	if ( linePosition != NULL )
		*linePosition = U;
	
	return intersection;
}
