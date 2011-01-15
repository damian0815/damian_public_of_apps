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
	void addVertex( float x, float y ) 
		{ addVertex( ofPoint( x,y ) ); }
	
	void addBezierVertex(ofPoint cp1, ofPoint cp2, ofPoint p);
	void addBezierVertex( float cp1x, float cp1y, float cp2x, float cp2y, float px, float py )
		{ addBezierVertex( ofPoint(cp1x,cp1y), ofPoint(cp2x,cp2y), ofPoint(px,py) ); }
	
	void addCurveVertex(ofPoint p);
	void addCurveVertex( float x, float y ) 
		{ addCurveVertex( ofPoint( x,y ) ); }

	void close();

	/// must call tessellate before calling draw, if the shape has changed
	void tessellate();
	void draw();
	
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
			type = OFSHAPE_SEG_BEZIER; points.push_back( c1 ); points.push_back( c2 ); points.push_back( p ); 
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
	
	ofPolyline cachedPolyline;
	ofVboMesh cachedTessellation;
	
};
