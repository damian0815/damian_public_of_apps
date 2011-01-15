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




void ofShape::setCurveResolution(int numPoints){
	resolution = numPoints;
}

void ofShape::close(){
	if ( segments.size()>0 )
	{
		if ( segments.front().getPoints().size()>1 ){
			// add the first vertex again
			segments.back().addVertex( segments.front().getPoints()[0] );
		}
	}
}


void ofShape::bezierSegmentToPolyline( const ofShapeSegment & seg, ofPolyline& polyline ){

	
	// if, and only if poly vertices has points, we can make a bezier
	// from the last point
	
	// the resolultion with which we computer this bezier
	// is arbitrary, can we possibly make it dynamic?
	
	const vector<ofPoint>& points = seg.getPoints();
	if ( points.size()>= 4 )
	{
		for ( int k=3; k<points.size(); k+=3 )
		{
			int k0 = k-3;
			int k1 = k-2;
			int k2 = k-1;
			int k3 = k;
/*			k0 = ofClamp(k0, 0, points.size()-1);
			k1 = ofClamp(k1, 0, points.size()-1);
			k2 = ofClamp(k2, 0, points.size()-1);
			k3 = ofClamp(k3, 0, points.size()-1);*/

			const ofPoint& p0 = points[k0];
			const ofPoint& p1 = points[k1];
			const ofPoint& p2 = points[k2];
			const ofPoint& p3 = points[k3];
			
			ofPoint a, b, c;

			float   t, t2, t3;
			float   x, y, z;
			
			// polynomial coefficients
			
/*			cx = 3.0f * (x1 - x0);
			cy = 3.0f * (y1 - y0);
			cz = 3.0f * (z1 - z0);*/
			c = 3.0f*(p1-p0);

/*			bx = 3.0f * (x2 - x1) - cx;
			by = 3.0f * (y2 - y1) - cy;
			bz = 3.0f * (z2 - z1) - cz;*/
			b = 3.0f*(p2-p1) - c;
			
/*			ax = x3 - x0 - cx - bx;
			ay = y3 - y0 - cy - by;
			az = z3 - z0 - cz - bz;*/
			a = p3 - p0 - c - b;

			
			for (int i = 0; i < resolution; i++){
				t  = (float)i / (float)(resolution-1);
				t2 = t * t;
				t3 = t2 * t;
				ofPoint newPoint = a*t3 + b*t2 + c*t + p0;
				/*
				x = (ax * t3) + (bx * t2) + (cx * t) + x0;
				y = (ay * t3) + (by * t2) + (cy * t) + y0;
				z = (az * t3) + (bz * t2) + (cz * t) + z0;*/
				polyline.addVertex( newPoint );
			}
			
		}
	}	
}



void ofShape::curveSegmentToPolyline( const ofShapeSegment & seg, ofPolyline& polyline ){
	if( seg.getPoints().size() == 0 )
		return;
	
	ofPoint p0, p1, p2, p3;
	
	vector<ofPoint> p = seg.getPoints();
	p.push_back(p.back());
	
	if ( p.size() >= 4 ){	
		
		for(int k = 0; k < p.size(); k++ ){
			
			float x,y,z;
			float t,t2,t3;
			
			int k0 = k-3;
			int k1 = k-2;
			int k2 = k-1;
			int k3 = k;
			
			k0 = ofClamp(k0, 0, p.size()-1);
			k1 = ofClamp(k1, 0, p.size()-1);
			k2 = ofClamp(k2, 0, p.size()-1);
			k3 = ofClamp(k3, 0, p.size()-1);
			
			ofPoint p0 = p[k0];
			ofPoint p1 = p[k1];
			ofPoint p2 = p[k2];
			ofPoint p3 = p[k3];
			
			for (int i = 0; i < resolution; i++){
				
				t 	=  (float)i / (float)(resolution-1);
				t2 	= t * t;
				t3 	= t2 * t;
				
				x = 0.5f * ( ( 2.0f * p1.x ) + ( -p0.x + p2.x ) * t + ( 2.0f * p0.x - 5.0f * p1.x + 4 * p2.x - p3.x ) * t2 + ( -p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x ) * t3 );
				y = 0.5f * ( ( 2.0f * p1.y ) + ( -p0.y + p2.y ) * t + ( 2.0f * p0.y - 5.0f * p1.y + 4 * p2.y - p3.y ) * t2 + ( -p0.y + 3.0f * p1.y - 3.0f * p2.y + p3.y ) * t3 );
				z = 0.5f * ( ( 2.0f * p1.z ) + ( -p0.z + p2.z ) * t + ( 2.0f * p0.z - 5.0f * p1.z + 4 * p2.z - p3.z ) * t2 + ( -p0.z + 3.0f * p1.z - 3.0f * p2.z + p3.z ) * t3 );
				
				polyline.addVertex( ofPoint( x, y, z ) );
			
			}
		}
	}
}

void ofShape::tessellate(){
	
	cachedPolyline.clear();
	
	if( segments.size() ){
		ofPoint tmpPt;
		
		for(int i = 0; i < segments.size(); i++){
			if( segments[i].getType() == OFSHAPE_SEG_LINE ){
				for(int j = 0; j < segments[i].getPoints().size(); j++){
					cachedPolyline.addVertex( segments[i].getPoints()[j] );
					//tessellator.addVertex(segments[i].points[j].x, segments[i].points[j].y, segments[i].points[j].z);
					printf(" polyline vertex(%f, %f, %f)\n", segments[i].getPoints()[j].x, segments[i].getPoints()[j].y, segments[i].getPoints()[j].z);
				}
			}else if( segments[i].getType() == OFSHAPE_SEG_BEZIER ){
				bezierSegmentToPolyline(segments[i], cachedPolyline);
			}else if( segments[i].getType() == OFSHAPE_SEG_CURVE ){
				curveSegmentToPolyline(segments[i], cachedPolyline);
			}
		}
		
		
		ofLog(OF_LOG_NOTICE, "calling tessellator");
		bool bIs2D = true;
		cachedTessellation = ofTessellator::tessellate( cachedPolyline, bIs2D );
		ofLog(OF_LOG_NOTICE, "called");

		//				
		//				printf("cachedPoints.size() is %i \n", cachedPoints.size());
		//				
		//				cachedPointsFloat.assign( cachedPoints.size() * 3, 0.0);
		//				
		//				int k = 0;
		//				for(int i = 0; i < cachedPoints.size(); i++){
		//					cachedPointsFloat[k]	= cachedPoints[i].x;
		//					cachedPointsFloat[k+1]	= cachedPoints[i].y;
		//					cachedPointsFloat[k+2]	= cachedPoints[i].z;
		//					printf("v is %f %f %f\n", cachedPoints[i].x, cachedPoints[i].y, cachedPoints[i].z);
		//					k+=3;
		//				}
	}
}

void ofShape::draw(){
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
	if ( segments.size() == 0 || segments.back().getType() != OFSHAPE_SEG_LINE )
		segments.push_back( ofShapeSegment( OFSHAPE_SEG_LINE ) );
	segments.back().addVertex( p );
}

void ofShape::addBezierVertex(ofPoint cp1, ofPoint cp2, ofPoint p){
	if ( segments.size() == 0 ) {
		segments.push_back( ofShapeSegment( OFSHAPE_SEG_BEZIER ) );
		// user has done something stupid -- let's be kind
		segments.back().addVertex( cp1 );
	}
	segments.back().addBezierVertex( cp1, cp2, p );	
}

void ofShape::addCurveVertex(ofPoint p){
	if ( segments.size() == 0 || segments.back().getType() != OFSHAPE_SEG_CURVE )
		segments.push_back( ofShapeSegment( OFSHAPE_SEG_CURVE ) );
	segments.back().addCurveVertex( p );	
}

