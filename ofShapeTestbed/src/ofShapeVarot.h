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
#include "ofColor.h"
#include "Path.h"
#include "Polyline.h"

/** ofPath
 
 An ofPath is a collection of ofPathSegments each of which can be polyline, curve, bezier. 
 
 An ofPath cannot have any gaps -- it is continuous.
 
 */

class ofPath {
public:
	ofPath();
	
	/// set winding mode; mode is one of OF_POLY_WINDING_ODD etc.
	void setPolyWindingMode( int mode ) { polyWindingMode = mode; }
	
	/// add a vertex, tracing a straight line from current position (if any) to pos
	void addVertex( float x, float y );
	void addVertex( const ofVec2f& pos ) { addVertex( pos.x, pos.y ); }
	/// add a curve vertex, creating a Catmull-Rom splile from current position to pos
	//void addCurveVertex( ofVec2f pos );
	/// add a bezier vertex, creating a Bezier spline from current position pos, using 
	/// startHandle as control point when leaving the current position, and endHandle 
	/// as control point when entering endPos.
	void addBezierVertex( float startHandleX, float startHandleY, float endHandleX, float endHandleY, float endPosX, float endPosY );
	void addBezierVertex( const ofVec2f& startHandle, const ofVec2f& endHandle, const ofVec2f& endPos ) { addBezierVertex( startHandle.x, startHandle.y, endHandle.x, endHandle.y, endPos.x, endPos.y ); }
	/// close the path
	void close();
	
	/// draw the path
	void draw();

	
private:
	
	int polyWindingMode;
	
	bool bFirstVertex;
	ofColor lineColor;
	bool bFilled;
	ofColor fillColor;
	
	// livarot storage
	Path thePath;
	
	
};


/**  ofShape
 
 ofShape is a collection of paths.
 
 */
 
class ofShape {
public:
	void draw();

private:
	
	vector<ofPath> paths;
	
};




/** ofPolyline
 
 Subclasses the livarot Polyline to receive output from the livarot Path.

 */

class ofPolyline: public Polyline
{
public:	
	ofPolyline() { bClosed = false; }
	
	int getNumPoints() const { return points.size(); }
	bool getIsClosed() const { return bClosed; }
	const ofVec2f& getPoint( int i ) const { return points[i]; }
	const ofVec2f& operator[]( int i ) const { return points[i]; }
	
	/// from livarot Polyline
	virtual void	moveto(MPoint<float> const &p, PathElem* origin);
	virtual void	lineto(MPoint<float> const &p, float stS, float enS, PathElem* origin);
	virtual	void	close(PathElem* origin);
	
	
private:

	vector<ofVec2f> points;
	bool bClosed;
	
};



