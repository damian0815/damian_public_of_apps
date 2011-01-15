/*
 *  ofShape.cpp
 *  openFrameworks
 *
 *  Created by theo on 28/10/2009.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#include "ofShape.h"
#include "ofTessellator.h"



/** 
 @TODO
	ofShape:
	- fix issues with multiple segment types
	- ofShapeCollection for multiple shapes inside (ofShape rename to ofPath)
	- ttf integration: ttf spits out ofShapeCollection
*/


void ofPolyline::draw() const {
	for ( int i=1; i<points.size(); i++ ) {
		ofLine( points[i-1], points[i] );
	}
}


ofShape::ofShape(){
	bFilled = ofGetStyle().bFill;
	resolution = 16;
	bNeedsTessellation = true;
	polyWindingMode = ofGetStyle().polyMode;
	lineColor = ofGetStyle().color;
	fillColor = ofGetStyle().color;
	bShouldClose = false;
}



void ofShape::setCurveResolution(int numPoints) {
	resolution = numPoints;
}

void ofShape::close() {
	bShouldClose = true;
}



void ofShape::bezierSegmentToPolyline( const ofShapeSegment & seg, ofPolyline& polyline ){

	
	// if, and only if poly vertices has points, we can make a bezier
	// from the last point
	
	// the resolultion with which we computer this bezier
	// is arbitrary, can we possibly make it dynamic?
	
	const vector<ofPoint>& points = seg.getPoints();
	if ( points.size()>= 4 ) {
		for ( int k=3; k<points.size(); k+=3 ) {
			int k0 = k-3;
			int k1 = k-2;
			int k2 = k-1;
			int k3 = k;

			const ofPoint& p0 = points[k0];
			const ofPoint& p1 = points[k1];
			const ofPoint& p2 = points[k2];
			const ofPoint& p3 = points[k3];
			
			ofPoint a, b, c;
			float   t, t2, t3;
			float   x, y, z;
			
			// polynomial coefficients			
			c = 3.0f*(p1-p0);
			b = 3.0f*(p2-p1) - c;
			a = p3 - p0 - c - b;
			
			for (int i = 0; i < resolution; i++){
				t  = (float)i / (float)(resolution-1);
				t2 = t * t;
				t3 = t2 * t;
				ofPoint newPoint = a*t3 + b*t2 + c*t + p0;
				polyline.addVertex( newPoint );
			}
			
		}
	}	
}



void ofShape::curveSegmentToPolyline( const ofShapeSegment & seg, ofPolyline& polyline ){
	
	if( seg.getPoints().size() == 0 )
		return;
	
	ofPoint p0, p1, p2, p3;
	
	const vector<ofPoint>& p = seg.getPoints();
	
	if ( p.size() >= 4 ){	
		for(int k = 3; k < p.size(); k++ ){
			float t,t2,t3;
			ofPoint p0 = p[k-3];
			ofPoint p1 = p[k-2];
			ofPoint p2 = p[k-1];
			ofPoint p3 = p[k  ];
			
			for (int i = 0; i < resolution; i++){
				t 	=  (float)i / (float)(resolution-1);
				t2 	= t * t;
				t3 	= t2 * t;

				ofPoint pt = 0.5f * ( (2.0f*p1) + (-p0+p2)*t + (2.0f*p0 - 5.0f*p1 + 4.0f*p2 - p3)*t2 + (-p0 + 3.0f*p1 - 3.0f*p2 + p3)*t3 );
				polyline.addVertex( pt );
			}
		}
	}
}

void ofShape::tessellate(){
	
//	ofLog(OF_LOG_NOTICE, "tessellate, %i segments", segments.size() );
	cachedPolyline.clear();
	
	if( segments.size() ){
		
		for(int i = 0; i < segments.size(); i++){
			if( segments[i].getType() == OFSHAPE_SEG_LINE ){
				for(int j = 0; j < segments[i].getPoints().size(); j++){
					cachedPolyline.addVertex( segments[i].getPoint(j) );
				}
			}else if( segments[i].getType() == OFSHAPE_SEG_BEZIER ){
				bezierSegmentToPolyline(segments[i], cachedPolyline);
			}else if( segments[i].getType() == OFSHAPE_SEG_CURVE ){
				curveSegmentToPolyline(segments[i], cachedPolyline);
			}
		}
		
		// close?
		if ( bShouldClose && cachedPolyline.size() > 0 ) {
			cachedPolyline.addVertex( cachedPolyline[0] );
		}
		
		bool bIs2D = true;
#ifdef DRAW_WITH_MESHIES
		cachedMeshies = ofTessellator::tessellate( cachedPolyline, polyWindingMode, bFilled, bIs2D );
#else
		cachedTessellation = ofTessellator::tessellate( cachedPolyline, polyWindingMode, bFilled, bIs2D );
#endif
	}
//	ofLog(OF_LOG_NOTICE, "tessellate done");
	
	bNeedsTessellation = false;
}

void ofShape::draw(){
	if ( bNeedsTessellation ){
		tessellate();
	}
	if ( bFilled ) {
#ifdef DRAW_WITH_MESHIES
		for ( int i=0; i<cachedMeshies.size(); i++ ) {
			ofSetColor( fillColor );
			glBegin( cachedMeshies[i].mode );
			for ( int j=0; j<cachedMeshies[i].vertices.size(); j++ ) {
				glVertex2f( cachedMeshies[i].vertices[j].x, cachedMeshies[i].vertices[j].y );
			}
			glEnd();
		}
#else
		cachedTessellation.drawVertices();
#endif
	}
	ofSetColor( lineColor );
	cachedPolyline.draw();
}


void ofShape::addVertex(ofPoint p){
	if ( segments.size() == 0 ) {
		segments.push_back( ofShapeSegment( OFSHAPE_SEG_LINE ) );
	}
	else {
		ofShapeSegment& backSeg = segments.back();
		if ( backSeg.getType() != OFSHAPE_SEG_LINE ) {
			// back segment is not a line
			if ( backSeg.getNumPoints() > 0 ) {
				// back segment has points
				if ( backSeg.getType() == OFSHAPE_SEG_CURVE ) {
					// back segment is a Catmull-Rom, so we must add the new point to the old curve
					// to prevent a gap in the line
					backSeg.addSegmentCurveVertex( p );
				}
			}
			// push back a new line segment
			segments.push_back( ofShapeSegment( OFSHAPE_SEG_LINE ) );
		}
	}
	segments.back().addSegmentVertex( p );
	bNeedsTessellation = true;
}

void ofShape::addBezierVertex(ofPoint cp1, ofPoint cp2, ofPoint p){
	if ( segments.size() == 0 || (segments.back().getType() != OFSHAPE_SEG_BEZIER && segments.back().getNumPoints()>1) ) {
		segments.push_back( ofShapeSegment( OFSHAPE_SEG_BEZIER ) );
		// user has done something stupid -- let's be kind
		segments.back().addSegmentVertex( cp1 );
	}
	segments.back().addSegmentBezierVertex( cp1, cp2, p );	
	bNeedsTessellation = true;
}


void ofShape::addCurveVertex(ofPoint p){
	if ( segments.size() == 0 ) {
		segments.push_back( ofShapeSegment( OFSHAPE_SEG_CURVE ) );
	}
	else {
		ofShapeSegment& backSeg = segments.back();
		if ( backSeg.getType() != OFSHAPE_SEG_CURVE ) {
			// back segment is not a curve
			// so we should start our new segment with the previous 2 points of the segment previous 
			// to us (for a line) or double up the last point (for a bezier segment)
			int ultimate, penultimate;
			ultimate = max(0,int(backSeg.getNumPoints())-1);
			if ( backSeg.getType() == OFSHAPE_SEG_BEZIER ) {
				penultimate = max(0,int(backSeg.getNumPoints())-2);
			}
			else { // OFSHAPE_SEG_LINE
				penultimate = max(0,int(backSeg.getNumPoints())-2);
			}
			ofShapeSegment newSeg = ofShapeSegment( OFSHAPE_SEG_CURVE );
			newSeg.addSegmentCurveVertex( backSeg.getPoints()[penultimate] );
			newSeg.addSegmentCurveVertex( backSeg.getPoints()[ultimate] );
			segments.push_back( newSeg );
		}
	}
	segments.back().addSegmentCurveVertex( p );	
	bNeedsTessellation = true;
}

