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

#define DRAW_WITH_MESHIES

#ifdef DRAW_WITH_MESHIES
typedef struct _meshy { GLint mode; vector<ofPoint>vertices; } meshy;
#endif


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



#include "ofTessellator.h"


/** ofShape
 
 Represents a 'shape'.
 
 */

class ofShape{
public:
	
	ofShape();
	void setCurveResolution(int numPoints);
	void clear() { segments.clear(); bNeedsTessellation = true; cachedPolyline.clear(); cachedTessellation.clear(); bShouldClose=false; }
		
	void addVertex(ofPoint p1);
	void addVertex( float x, float y ) 
		{ addVertex( ofPoint( x,y ) ); }
	
	void addBezierVertex(ofPoint cp1, ofPoint cp2, ofPoint p);
	void addBezierVertex( float cp1x, float cp1y, float cp2x, float cp2y, float px, float py )
		{ addBezierVertex( ofPoint(cp1x,cp1y), ofPoint(cp2x,cp2y), ofPoint(px,py) ); }
	
	void addCurveVertex(ofPoint p);
	void addCurveVertex( float x, float y ) 
		{ addCurveVertex( ofPoint( x,y ) ); }

	/// close the shape
	void close();
	/// next contour
	void nextContour() {};

	/// must call tessellate before calling draw, if the shape has changed
	void tessellate();
	void draw();
	
	/// drawing style
	/// polygon winding mode for tessellation
	void setPolyWindingMode( int newMode ) { polyWindingMode = newMode; bNeedsTessellation = true; }
	/// filled/outline
	void setFilled( bool bFill ) { bFilled = bFill; bNeedsTessellation = true; }
	/// set line + fill color simultaneously
	void setColor( const ofColor& color ) { setFillColor( color ); setLineColor( color ); }
	void setHexColor( int hex ) { setColor( ofColor().fromHex( hex ) ); };
	/// set line color
	void setLineColor( const ofColor& color ) { lineColor = color; }
	void setLineHexColor( int hex ) { setLineColor( ofColor().fromHex( hex ) ); };
	/// set fill color
	void setFillColor( const ofColor& color ) { fillColor = color; }
	void setFillHexColor( int hex ) { setFillColor( ofColor().fromHex( hex ) ); };
	
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
		void addSegmentVertex(const ofPoint& p) {
			points.push_back(p)	;
		}
		void addSegmentCurveVertex(const ofPoint& p) {
			type = OFSHAPE_SEG_CURVE; points.push_back(p);
		}
		void addSegmentBezierVertex(const ofPoint& c1, const ofPoint& c2, const ofPoint& p) {
			type = OFSHAPE_SEG_BEZIER; points.push_back( c1 ); points.push_back( c2 ); points.push_back( p ); 
		}

		int getType() const { return type; }
		const vector<ofPoint>& getPoints() const { return points; }
		const ofPoint& getPoint( int index ) const { return points[index]; }
		size_t getNumPoints() const { return points.size(); }
	private:
		segmentType type;
		vector<ofPoint> points;
	};
	
	
	void curveSegmentToPolyline(const ofShapeSegment & seg, ofPolyline& polyline);
	void bezierSegmentToPolyline(const ofShapeSegment & seg, ofPolyline& polyline);
	
	ofColor lineColor;
	bool bFilled;
	ofColor fillColor;
	
	// true if this shape should be closed
	bool bShouldClose;
	
	int resolution;
	vector <ofShapeSegment> segments;
	
	int polyWindingMode;
	bool bNeedsTessellation;
	ofPolyline cachedPolyline;
	ofVboMesh cachedTessellation;
#ifdef DRAW_WITH_MESHIES
	vector<meshy> cachedMeshies;
#endif
	
};
