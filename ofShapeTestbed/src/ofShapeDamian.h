/*
 *  ofShape.h
 *  openFrameworksLib
 *
 *  Created by damian on 13/01/11.
 *  Copyright 2011 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "ofVec2f.h"

class ofPolyline;


/** ofPathPoint
 
 Base class for points in an ofPath.
 
 */

class ofPathPoint
{
public:
	/// the position of the point
	ofVec2f pos;

	/// only for Beziers
	/// tracing the path from start to finish, inHandle is the bezier handle coming in to this point
	ofVec2f inHandle;
	/// tracing the path from start to finish, outHandle is the bezier handle out of this point to the next one
	ofVec2f outHandle;
};


/** ofPathSegment
 
 An ofPathSegment is a single segment from an ofPath. It can only be of one type (poly, curve, bezier).
 
 */

typedef enum {
	OF_PATHSEGMENT_POLY,	// line made of straight edges
	OF_PATHSEGMENT_CURVE,	// Catmull-Rom spline
	OF_PATHSEGMENT_BEZIER,	// Bezier spline
} ofPathSegmentType;

class ofPathSegment
{
public:
	ofPathSegment() { type = OF_PATHSEGMENT_POLY; }
	~ofPathSegment() {};
	
	void setType( ofPathSegmentType _type ) { type = _type; }
	void addVertex( ofVec2f vert );
	
private:
	
	vector<ofVec2f> rawPoints;
	ofPathSegmentType type;
};



/** ofPath
 
 An ofPath is a collection of ofPathSegments each of which can be polyline, curve, bezier. 
 
 An ofPath cannot have any gaps -- it is continuous.
 
 */

class ofPath {
public:
	ofPath() { bClosed = false; bFilled = false; }
	~ofPath(); 
	
	ofPolyline convertToPolyline();
	
	/// add a vertex, tracing a straight line from current position (if any) to pos
	void addVertex( ofVec2f pos );
	/// add a curve vertex, creating a Catmull-Rom splile from current position to pos
	void addCurveVertex( ofVec2f pos );
	void addBezierVertex( ofVec2f startHandle, ofVec2f endHandle, ofVec2f endPos );
	
	void closePath();

	
private:
	// this path is composed of segments
	vector<ofPathSegment> segments;
	
	
	ofColor lineColor;

	bool bFilled;
	ofColor fillColor;
	
	bool bClosed;
	
	ofVec2f currentPos;
	ofPathSegmentType currentType;
};


/*  ofShape
 
 ofShape is a collection of paths.
 
 */
 
class ofShape {
public:
	void draw();

private:
	
	vector<ofPath> paths;
	
};




