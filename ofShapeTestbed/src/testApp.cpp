#include "testApp.h"

#include "ofShape.h"

//--------------------------------------------------------------
void testApp::setup(){

	
	ofSetLogLevel( OF_LOG_NOTICE );
	ofSetFrameRate( 24 );
	ofSetVerticalSync(true);

	
	
	nCurveVertexes = 7;
	
	curveVertices[0].x = 326;
	curveVertices[0].y = 209;
	curveVertices[1].x = 306;
	curveVertices[1].y = 279;
	curveVertices[2].x = 265;
	curveVertices[2].y = 331;
	curveVertices[3].x = 304;
	curveVertices[3].y = 383;
	curveVertices[4].x = 374;
	curveVertices[4].y = 383;
	curveVertices[5].x = 418;
	curveVertices[5].y = 309;
	curveVertices[6].x = 345;
	curveVertices[6].y = 279;
	
	for (int i = 0; i < nCurveVertexes; i++){
		curveVertices[i].bOver 			= false;
		curveVertices[i].bBeingDragged 	= false;
		curveVertices[i].radius = 4;
	}
	
	
}

//--------------------------------------------------------------
void testApp::update(){

}


//--------------------------------------------------------------
void testApp::draw(){
	ofBackground( 255, 255, 255 );
	
	ofFill();
	ofSetHexColor(0xe0be21);
	
	//------(a)--------------------------------------
	// 
	// 		draw a star
	//
	// 		use poly winding odd, the default rule
	//
	// 		info about the winding rules is here:
	//		http://glprogramming.com/red/images/Image128.gif
	// 
	ofSetPolyMode(OF_POLY_WINDING_ODD);	// this is the normal mode
	ofShape s;
	s.addVertex(200,135);
	s.addVertex(15,135);
	s.addVertex(165,25);
	s.addVertex(105,200);
	s.addVertex(50,25);
	s.draw();
	
	
	//------(b)--------------------------------------
	// 
	// 		draw a star
	//
	// 		use poly winding nonzero
	//
	// 		info about the winding rules is here:
	//		http://glprogramming.com/red/images/Image128.gif
	// 
	s.clear();
	s.setHexColor(0xb5de10);
	s.setPolyWindingMode(OF_POLY_WINDING_NONZERO);
	s.addVertex(400,135);
	s.addVertex(215,135);
	s.addVertex(365,25);
	s.addVertex(305,200);
	s.addVertex(250,25);
	s.draw();
	//-------------------------------------
	
	
	
	//------(c)--------------------------------------
	// 
	// 		draw a star dynamically
	//
	// 		use the mouse position as a pct
	//		to calc nPoints and internal point radius
	//
	float xPct = (float)(mouseX) / (float)(ofGetWidth());
	float yPct = (float)(mouseY) / (float)(ofGetHeight());
	int nTips = 5 + xPct * 60;
	int nStarPts = nTips * 2;
	float angleChangePerPt = TWO_PI / (float)nStarPts;
	float innerRadius = 0 + yPct*80;
	float outerRadius = 80;
	float origx = 525;
	float origy = 100;
	float angle = 0;
	
	s.clear();
	s.setHexColor(0xa16bca);
	for (int i = 0; i < nStarPts; i++){
		if (i % 2 == 0) {
			// inside point:
			float x = origx + innerRadius * cos(angle);
			float y = origy + innerRadius * sin(angle);
			s.addVertex(x,y);
		} else {
			// outside point
			float x = origx + outerRadius * cos(angle);
			float y = origy + outerRadius * sin(angle);
			s.addVertex(x,y);
		}
		angle += angleChangePerPt;
	}
	s.draw();
	//-------------------------------------
	
	//------(d)--------------------------------------
	// 
	// 		poylgon of random points
	//
	// 		lots of self intersection, 500 pts is a good stress test
	// 
	// 
	s.clear();
	s.setHexColor(0x0cb0b6);
	s.setPolyWindingMode(OF_POLY_WINDING_ODD);
	for (int i = 0; i < 10; i++){
		s.addVertex(ofRandom(650,850), ofRandom(20,200));
	}
	s.draw();
	//-------------------------------------
	
	
	//------(e)--------------------------------------
	// 
	// 		use sin cos and time to make some spirally shape
	//
	glPushMatrix();
	glTranslatef(100,300,0);
	s.clear();
	s.setHexColor(0xff2220);
	s.setFilled( true );
	s.setPolyWindingMode(OF_POLY_WINDING_ODD);
	float angleStep 	= TWO_PI/(100.0f + sin(ofGetElapsedTimef()/5.0f) * 60); 
	float radiusAdder 	= 0.5f;
	float radius 		= 0;
	for (int i = 0; i < 200; i++){
		float anglef = (i) * angleStep;
		float x = radius * cos(anglef);
		float y = radius * sin(anglef); 
		s.addVertex(x,y);
		radius 	+= radiusAdder; 
	}
	s.close();
	s.draw();
	glPopMatrix();
	//-------------------------------------
	
	//------(f)--------------------------------------
	// 
	// 		ofCurveVertex
	// 
	// 		because it uses catmul rom splines, we need to repeat the first and last 
	// 		items so the curve actually goes through those points
	//

	s.clear();
	s.setFillColor(0x2bdbe6);
	
	for (int i = 0; i < nCurveVertexes; i++){
		
		
		// sorry about all the if/states here, but to do catmull rom curves
		// we need to duplicate the start and end points so the curve acutally 
		// goes through them.
		
		// for i == 0, we just call the vertex twice
		// for i == nCurveVertexes-1 (last point) we call vertex 0 twice
		// otherwise just normal ofCurveVertex call
		
		if (i == 0){
			s.addCurveVertex(curveVertices[0].x, curveVertices[0].y); // we need to duplicate 0 for the curve to start at point 0
			s.addCurveVertex(curveVertices[0].x, curveVertices[0].y); // we need to duplicate 0 for the curve to start at point 0
		} else if (i == nCurveVertexes-1){
			s.addCurveVertex(curveVertices[i].x, curveVertices[i].y);
			s.addCurveVertex(curveVertices[0].x, curveVertices[0].y);	// to draw a curve from pt 6 to pt 0
			s.addCurveVertex(curveVertices[0].x, curveVertices[0].y);	// we duplicate the first point twice
		} else {
			s.addCurveVertex(curveVertices[i].x, curveVertices[i].y);
		}
	}
	s.draw();
	
	
	// show a faint the non-curve version of the same polygon:
	ofEnableAlphaBlending();
	s.setFilled( false );
	s.setLineColor( ofColor(0,0,0,40) );
	for (int i = 0; i < nCurveVertexes; i++){
		s.addVertex(curveVertices[i].x, curveVertices[i].y);
	}
	s.close();
	s.draw();
	
	
	ofSetColor(0,0,0,80);
	for (int i = 0; i < nCurveVertexes; i++){
		if (curveVertices[i].bOver == true) ofFill();
		else ofNoFill();
		ofCircle(curveVertices[i].x, curveVertices[i].y,4);
	}
	ofDisableAlphaBlending();
	//-------------------------------------
	
	
	//------(g)--------------------------------------
	// 
	// 		ofBezierVertex
	// 
	// 		with ofBezierVertex we can draw a curve from the current vertex
	//		through the the next three vertexes we pass in.
	//		(two control points and the final bezier point)
	//		
	
	float x0 = 500;
	float y0 = 300;
	float x1 = 550+50*cos(ofGetElapsedTimef()*1.0f);
	float y1 = 300+100*sin(ofGetElapsedTimef()/3.5f);
	float x2 = 600+30*cos(ofGetElapsedTimef()*2.0f);
	float y2 = 300+100*sin(ofGetElapsedTimef());
	float x3 = 650;
	float y3 = 300;
	
	
	
	s.clear();
	s.setFilled(true);
	s.setHexColor(0xFF9933);
	s.addVertex(x0,y0);
	s.addBezierVertex(x1,y1,x2,y2,x3,y3);
	s.draw();
	
	
	ofEnableAlphaBlending();
	ofFill();
	ofSetColor(0,0,0,40);
	ofCircle(x0,y0,4);
	ofCircle(x1,y1,4);
	ofCircle(x2,y2,4);
	ofCircle(x3,y3,4);
	ofDisableAlphaBlending();
	
	
	
	//------(h)--------------------------------------
	// 
	// 		holes / ofNextContour
	// 
	// 		with ofNextContour we can create multi-contour shapes
	// 		this allows us to draw holes, for example... 
	//
	ofSetHexColor(0xd3ffd3);
	ofRect(80,480,140,70);
	s.clear();
	s.setFilled(true);
	s.setHexColor( 0xff00ff );
	s.addVertex(100,500);
	s.addVertex(180,550);
	s.addVertex(100,600);
	s.close();
	s.nextContour();
	s.addVertex(120,520);
	s.addVertex(160,550);
	s.addVertex(120,580);
	s.close();
	s.draw();
	//-------------------------------------
	
	
	//------(i)--------------------------------------
	// 
	// 		CSG / ofNextContour
	// 
	// 		with different winding rules, you can even use ofNextContour to 
	// 		perform constructive solid geometry 
	// 		
	// 		be careful, the clockwiseness or counter clockwisenss of your multiple
	// 		contours matters with these winding rules.
	//
	// 		for csg ideas, see : http://glprogramming.com/red/chapter11.html
	// 
	// 		info about the winding rules is here:
	//		http://glprogramming.com/red/images/Image128.gif
	// 
	s.clear();
	s.setFilled(false);
	
	glPushMatrix();
	
	ofSetPolyMode(OF_POLY_WINDING_ODD);
	
	s.addVertex(300,500);
	s.addVertex(380,550);
	s.addVertex(300,600);
	s.close();
	s.nextContour();
	
	for (int i = 0; i < 20; i++){
		float anglef = ((float)i / 19.0f) * TWO_PI;
		float x = 340 + 30 * cos(anglef);
		float y = 550 + 30 * sin(anglef); 
		s.addVertex(x,y);
		radius 	+= radiusAdder; 
	}
	
	s.close();
	s.draw();
	
	glTranslatef(100,0,0);
	
	s.clear();
	s.setPolyWindingMode(OF_POLY_WINDING_NONZERO);
	
	s.addVertex(300,500);
	s.addVertex(380,550);
	s.addVertex(300,600);

	s.close();
	s.nextContour();
	
	for (int i = 0; i < 20; i++){
		float anglef = ((float)i / 19.0f) * TWO_PI;
		float x = 340 + 30 * cos(anglef);
		float y = 550 + 30 * sin(anglef); 
		s.addVertex(x,y);
		radius 	+= radiusAdder; 
	}

	s.close();
	s.draw();
	
	glTranslatef(100,0,0);
	s.clear();
	s.setPolyWindingMode(OF_POLY_WINDING_ABS_GEQ_TWO);
	s.addVertex(300,500);
	s.addVertex(380,550);
	s.addVertex(300,600);
	s.close();
	s.nextContour();
	
	for (int i = 0; i < 20; i++){
		float anglef = ((float)i / 19.0f) * TWO_PI;
		float x = 340 + 30 * cos(anglef);
		float y = 550 + 30 * sin(anglef); 
		s.addVertex(x,y);
		radius 	+= radiusAdder; 
	}

	s.close();
	s.draw();
	
	glPopMatrix();
	//-------------------------------------
	
	
	ofSetHexColor(0x000000);
	ofDrawBitmapString("(a) star\nwinding rule odd", 20,210);
	
	ofSetHexColor(0x000000);
	ofDrawBitmapString("(b) star\nwinding rule nonzero", 220,210);
	
	ofSetHexColor(0x000000);
	ofDrawBitmapString("(c) dynamically\ncreated shape", 420,210);
	
	ofSetHexColor(0x000000);
	ofDrawBitmapString("(d) random points\npoly", 670,210);
	
	ofSetHexColor(0x000000);
	ofDrawBitmapString("(e) fun with sin/cos", 20,410);
	
	ofSetHexColor(0x000000);
	ofDrawBitmapString("(f) ofCurveVertex\nuses catmull rom\nto make curved shapes", 220,410);
	
	ofSetHexColor(0x000000);
	ofDrawBitmapString("(g) ofBezierVertex\nuses bezier to draw curves", 460,410);
	
	
	ofSetHexColor(0x000000);
	ofDrawBitmapString("(h) ofNextContour\nallows for holes", 20,610);
	
	ofSetHexColor(0x000000);
	ofDrawBitmapString("(i) ofNextContour\ncan even be used for CSG operations\nsuch as union and intersection", 260,620);
	
	
	
	
	
		
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//------------- -------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	for (int i = 0; i < nCurveVertexes; i++){
		float diffx = x - curveVertices[i].x;
		float diffy = y - curveVertices[i].y;
		float dist = sqrt(diffx*diffx + diffy*diffy);
		if (dist < curveVertices[i].radius){
			curveVertices[i].bOver = true;
		} else {
			curveVertices[i].bOver = false;
		}	
	}
}


//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	for (int i = 0; i < nCurveVertexes; i++){
		if (curveVertices[i].bBeingDragged == true){
			curveVertices[i].x = x;
			curveVertices[i].y = y;
		}
	}
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	for (int i = 0; i < nCurveVertexes; i++){
		float diffx = x - curveVertices[i].x;
		float diffy = y - curveVertices[i].y;
		float dist = sqrt(diffx*diffx + diffy*diffy);
		if (dist < curveVertices[i].radius){
			curveVertices[i].bBeingDragged = true;
		} else {
			curveVertices[i].bBeingDragged = false;
		}	
	}
	
	
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	
	for (int i = 0; i < nCurveVertexes; i++){
		curveVertices[i].bBeingDragged = false;	
	}
}


//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

