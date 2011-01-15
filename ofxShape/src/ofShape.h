/*
 *  ofShape.h
 *  openFrameworks
 *
 *  Created by theo on 28/10/2009.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include "ofMain.h"
#include "ofShapeUtils.h"
#include "ofVboMesh.h"

/** ofPolyline
 
 A line composed of straight line segments.
 
 */


class ofPolyline {
public:	
	
	void addVertex( const ofPoint& p ) { points.push_back(p); }
	
	size_t size() const { return points.size(); }
	const ofPoint& operator[] (int index) const { return points[index]; }
	
	void draw();
	
private:
	vector<ofPoint> points;
	
};


/** ofShape
 
 Represents a 'shape'.
 
 */

class ofShape{
public:
	
	ofShape();
	void setCurveResolution(int numPoints);
		
	void addVertex(ofPoint p1);
	void addBezierVertex(ofPoint cp1, ofPoint cp2, ofPoint p2);
	void addCurveVertex(ofPoint p);

	/// must call tesselate before calling draw
	void tessellate();
	void draw( float x, float y, float z=0);
	
private:		
				
	void curveSegmentToPolyline(ofShapeSegment & seg, vector<ofPoint>& polyline);
	void bezierSegmentToPolyline(ofShapeSegment & seg, vector<ofPoint>& polyline);
	
	int resolution;
	vector <ofShapeSegment> segments;
	vector <ofPoint> lastTwoPoints;
	
	ofVboMesh cachedTessellation;
	
};
