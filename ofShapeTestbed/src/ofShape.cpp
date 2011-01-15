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




void ofPolyline::draw() const {
	for ( int i=1; i<points.size(); i++ ) {
		ofLine( points[i-1], points[i] );
	}
}


ofShape::ofShape(){
	bFilled = false;
	resolution = 16;
	bNeedsTessellation = true;
	lineColor = ofColor::gray;
	fillColor = ofColor::green;
}

/*
void ofShape::addVertex(ofPoint p1){
	
	if( segments.empty() || segments.back().getType() != OFXSHAPE_SEG_LINE ){
		segments.push_back( ofShapeSegment() );
	}
	
	segments.back().addPoint(p1);
}

void ofShape::addBezierVertex(ofPoint cp1, ofPoint cp2, ofPoint p2){
	
	if ( segments.empty() || segments.back().getType() != OFXSHAPE_SEG_BEZIER || segments.back().getNumPoints() == 0 ) {
		//you need at least one point before a bezier
		return;
	}

	segments.back().addControlPoint(cp1);
	segments.back().addControlPoint(cp2);
	segments.back().addPoint(p2);
}

void ofShape::addCurveVertex(ofPoint p){
	if( segments.empty() || segments.back().getType() != OFXSHAPE_SEG_CURVE ){
		segments.push_back( ofShapeSegment() );
	}
	
	segments.back().addCurvePoint(p);
}*/




void ofShape::setCurveResolution(int numPoints) {
	resolution = numPoints;
}

void ofShape::close() {
	if ( segments.size()>0 ) {
		if ( segments.front().getPoints().size()>0 ) {
			// add the first vertex again
			addVertex( segments.front().getPoints()[0] );
		}
	}
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
		ofPoint tmpPt;
		
		for(int i = 0; i < segments.size(); i++){
			if( segments[i].getType() == OFSHAPE_SEG_LINE ){
//				ofLog(OF_LOG_NOTICE, "line   segment, %3i points", segments[i].getPoints().size() );
				for(int j = 0; j < segments[i].getPoints().size(); j++){
					cachedPolyline.addVertex( segments[i].getPoints()[j] );
				}
			}else if( segments[i].getType() == OFSHAPE_SEG_BEZIER ){
//				ofLog(OF_LOG_NOTICE, "bezier segment, %3i points", segments[i].getPoints().size() );
				bezierSegmentToPolyline(segments[i], cachedPolyline);
			}else if( segments[i].getType() == OFSHAPE_SEG_CURVE ){
//				ofLog(OF_LOG_NOTICE, "curve  segment, %3i points", segments[i].getPoints().size() );
				curveSegmentToPolyline(segments[i], cachedPolyline);
			}
		}
		
		bool bIs2D = true;
		cachedTessellation = ofTessellator::tessellate( cachedPolyline, bIs2D );
	}
//	ofLog(OF_LOG_NOTICE, "tessellate done");
	
	bNeedsTessellation = false;
}

void ofShape::draw(){
	if ( bNeedsTessellation ){
		tessellate();
	}
	if ( bFilled ) {
		ofSetColor( fillColor );
		cachedTessellation.drawVertices();
	}
	else {
		ofSetColor( lineColor );
		cachedPolyline.draw();
	}
}


void ofShape::addVertex(ofPoint p){
	if ( segments.size() == 0 || segments.back().getType() != OFSHAPE_SEG_LINE ) {
		segments.push_back( ofShapeSegment( OFSHAPE_SEG_LINE ) );
	}
	segments.back().addVertex( p );
	bNeedsTessellation = true;
}

void ofShape::addBezierVertex(ofPoint cp1, ofPoint cp2, ofPoint p){
	if ( segments.size() == 0 ) {
		segments.push_back( ofShapeSegment( OFSHAPE_SEG_BEZIER ) );
		// user has done something stupid -- let's be kind
		segments.back().addVertex( cp1 );
	}
	segments.back().addBezierVertex( cp1, cp2, p );	
	bNeedsTessellation = true;
}

void ofShape::addCurveVertex(ofPoint p){
	if ( segments.size() == 0 ) {
		segments.push_back( ofShapeSegment( OFSHAPE_SEG_CURVE ) );
	}
	segments.back().addCurveVertex( p );	
	bNeedsTessellation = true;
}

