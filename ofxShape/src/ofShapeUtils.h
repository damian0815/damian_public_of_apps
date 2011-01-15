/*
 *  ofShapeUtils.h
 *  openFrameworks
 *
 *  Created by theo on 28/10/2009.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once
#include "ofMain.h"

typedef enum{
	OFXSHAPE_POINT_VERTEX,
	OFXSHAPE_CURVE_VERTEX,
	OFXSHAPE_CONTROL_VERTEX,
}pointType;

class ofShapePoint : public ofPoint{
	public:
	
	ofShapePoint(ofPoint p, pointType t){
		x = p.x;
		y = p.y;
		z = p.z;
		
		type = t;
	}
	

	pointType type;
};

typedef enum{
 OFXSHAPE_SEG_LINE,
 OFXSHAPE_SEG_BEZIER,
 OFXSHAPE_SEG_CURVE
}segmentType;

class ofShapeSegment{
	public: 
	
	ofShapeSegment(){
		closing = false;
		type = OFXSHAPE_SEG_LINE;
	}
	
	void addPoint(ofPoint p){
		points.push_back(ofShapePoint(p, OFXSHAPE_POINT_VERTEX));
	}
	
	void addControlPoint(ofPoint cp){
		points.push_back(ofShapePoint(cp, OFXSHAPE_CONTROL_VERTEX));
		type = OFXSHAPE_SEG_BEZIER;
	}
	
	void addCurvePoint(ofPoint p){
		points.push_back(ofShapePoint(p, OFXSHAPE_CURVE_VERTEX));
		type = OFXSHAPE_SEG_CURVE;
	}
	
	segmentType type;
	vector <ofShapePoint> points;
	bool closing;
};
