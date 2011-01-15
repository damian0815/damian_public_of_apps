/*
 *  ofShape.cpp
 *  openFrameworksLib
 *
 *  Created by damian on 13/01/11.
 *  Copyright 2011 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "ofShape.h"
#include "ofMain.h"
#include "VarotPolygon.h"

ofPath::ofPath(){ 
	bFirstVertex = true; 
	lineColor = ofColor::gray;
	fillColor = ofColor::black; 
	bFilled = false;
	polyWindingMode = OF_POLY_WINDING_ODD;
}
	

void ofPath::addVertex( float x, float y ) {
	MPoint<float> p( x, y );
	if ( bFirstVertex ) {
		thePath.moveto( p );
		bFirstVertex = false;
	}
	else {
		thePath.lineto( p );
	}
}

void ofPath::addBezierVertex( float startHandleX, float startHandleY, float endHandleX, float endHandleY, float endPosX, float endPosY ) {
	if ( bFirstVertex ){
		ofLog(OF_LOG_ERROR, "first vertex of a path must be an ordinary vertex: use addVertex() not addBezierVertex()" );
	}
	else {
	thePath.curveto(MPoint<float>(startHandleX, startHandleY), 
					MPoint<float>(endHandleX, endHandleY), 
					MPoint<float>(endPosX, endPosY));					
	}
}

void ofPath::close() {
	thePath.close();
}


void ofPath::draw() {

	VarotPolygon polygon;
	// convert thePath to a polygon using the appropriate winding rule
	polygon.setSource( &thePath, /* is A in boolean ops */ true );
	// delta controls resulting polygon smoothness
	float delta = 1.0f;
	thePath.fill( &polygon, delta );
	polygon.uncross( varotMode, /* redo path */ true );

	// get the path created by the polygon unwinding
	Path* tempPath = polygon.givePath();
	// we now own tempPath
	
	// convert thePath to a polyline and fill our polyline structure
	ofPolyline line;
	tempPath->fill( &line, delta );
	// we're done with tempPath
	delete tempPath;

	// draw it
	ofSetColor( lineColor );
	for ( int i=1; i<line.getNumPoints(); i++ ) {
		ofLine( line[i-1], line[i] );
	}
}


void ofPolyline::moveto(MPoint<float> const &p, PathElem* origin){
	ofLog(OF_LOG_NOTICE, "p%3i: %7.3f %7.3f", 0, p._x, p._y );
	points.push_back( ofVec2f( p._x, p._y ) );
}

void ofPolyline::lineto(MPoint<float> const &p, float stS, float enS, PathElem* origin){
	ofLog(OF_LOG_NOTICE, "p%3i: %7.3f %7.3f (%7.3f %7.3f)", points.size(), p._x, p._y, stS, enS );
	points.push_back( ofVec2f( p._x, p._y ) );
}

void ofPolyline::close(PathElem* origin){
	ofLog(OF_LOG_NOTICE, "p%3i: close", points.size() );
	bClosed = true;
}
