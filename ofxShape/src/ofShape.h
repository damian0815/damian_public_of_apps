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
	/// remove all the points
	void clear() { points.clear(); }

	/// add a vertex
	void addVertex( const ofPoint& p ) { points.push_back(p); }
	
	/// draw as line segments, with the current line style
	void draw() const;

	size_t size() const { return points.size(); }
	const ofPoint& operator[] (int index) const { return points[index]; }
	
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

	/// must call tessellate before calling draw, if the shape has changed
	void tessellate();
	void draw( float x, float y, float z=0);
	
private:		
	
	typedef enum {
		OFSHAPE_SEG_LINE,
		OFSHAPE_SEG_BEZIER,
		OFSHAPE_SEG_CURVE
	} segmentType;
	
	class ofShapeSegment {
	public: 
		
		ofShapeSegment( segmentType _type ){
			type = _type;
		}
		
		/// up to you to call the correct function
		void addVertex(const ofPoint& p) {
			points.push_back(p)	;
		}
		void addCurveVertex(const ofPoint& p) {
			points.push_back(p);
		}
		void addBezierVertex(const ofPoint& c1, const ofPoint& c2, const ofPoint& p) {
			points.push_back( c1 ); points.push_back( c2 ); points.push_back( p ); 
		}

		int getType() const { return type; }
		const vector<ofPoint>& getPoints() const { return points; }
	private:
		segmentType type;
		vector<ofPoint> points;
	};
	
	
	void curveSegmentToPolyline(const ofShapeSegment & seg, ofPolyline& polyline);
	void bezierSegmentToPolyline(const ofShapeSegment & seg, ofPolyline& polyline);
	
	ofColor lineColor;
	bool bFilled;
	ofColor fillColor;
	
	int resolution;
	vector <ofShapeSegment> segments;
	vector <ofPoint> lastTwoPoints;
	
	ofPolyline cachedPolyline;
	ofVboMesh cachedTessellation;
	
};
