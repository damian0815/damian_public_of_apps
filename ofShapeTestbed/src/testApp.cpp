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
	ofBeginShape();
	ofVertex(200,135);
	ofVertex(15,135);
	ofVertex(165,25);
	ofVertex(105,200);
	ofVertex(50,25);
	ofEndShape();

	// now using ofPath
	ofPushMatrix( );
	ofTranslate( 0, 100, 0 ); 
	ofShape starOdd;
//	starOdd.setPolyWindingMode( OF_POLY_WINDING_ODD );
	starOdd.addVertex(200,135);
	starOdd.addVertex(15,135);
	starOdd.addVertex(165,25);
	starOdd.addVertex(105,200);
	starOdd.addVertex(50,25);
	starOdd.close();
	starOdd.tessellate();
	starOdd.draw();
	ofPopMatrix();
	
	
	//------(b)--------------------------------------
	// 
	// 		draw a star
	//
	// 		use poly winding nonzero
	//
	// 		info about the winding rules is here:
	//		http://glprogramming.com/red/images/Image128.gif
	// 
	ofSetHexColor(0xb5de10);
	ofSetPolyMode(OF_POLY_WINDING_NONZERO);
	ofBeginShape();
	ofVertex(400,135);
	ofVertex(215,135);
	ofVertex(365,25);
	ofVertex(305,200);
	ofVertex(250,25);
	ofEndShape();
	
	// now using ofPath
	ofPushMatrix( );
	ofTranslate( 0, 100, 0 ); 
	ofSetHexColor(0xb5de10);
	ofShape starNonZero;
//	starNonZero.setPolyWindingMode( OF_POLY_WINDING_NONZERO );
	starNonZero.addVertex(400,135);
	starNonZero.addVertex(215,135);
	starNonZero.addVertex(365,25);
	starNonZero.addVertex(305,200);
	starNonZero.addVertex(250,25);
	starNonZero.close();
	starNonZero.draw();
	ofPopMatrix();
	
	//-------------------------------------
	
	
	
	//------(f)--------------------------------------
	// 
	// 		ofCurveVertex
	// 
	// 		because it uses catmul rom splines, we need to repeat the first and last 
	// 		items so the curve actually goes through those points
	//
	
	ofSetHexColor(0x2bdbe6);
	ofBeginShape();
	
	for (int i = 0; i < nCurveVertexes; i++){
		
		
		// sorry about all the if/states here, but to do catmull rom curves
		// we need to duplicate the start and end points so the curve acutally 
		// goes through them.
		
		// for i == 0, we just call the vertex twice
		// for i == nCurveVertexes-1 (last point) we call vertex 0 twice
		// otherwise just normal ofCurveVertex call
		
		if (i == 0){
			ofCurveVertex(curveVertices[0].x, curveVertices[0].y); // we need to duplicate 0 for the curve to start at point 0
			ofCurveVertex(curveVertices[0].x, curveVertices[0].y); // we need to duplicate 0 for the curve to start at point 0
		} else if (i == nCurveVertexes-1){
			ofCurveVertex(curveVertices[i].x, curveVertices[i].y);
			ofCurveVertex(curveVertices[0].x, curveVertices[0].y);	// to draw a curve from pt 6 to pt 0
			ofCurveVertex(curveVertices[0].x, curveVertices[0].y);	// we duplicate the first point twice
		} else {
			ofCurveVertex(curveVertices[i].x, curveVertices[i].y);
		}
	}
	
	ofEndShape();

	
	// now with ofShape
	ofPushMatrix();
	ofTranslate( 0, 200, 0 );
	ofShape s;
	for (int i = 0; i < nCurveVertexes; i++){
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
//	s.close();
	s.tessellate();
	s.draw();
	ofPopMatrix();
	
	
	// show a faint the non-curve version of the same polygon:
	ofEnableAlphaBlending();
	ofNoFill();
	ofSetColor(0,0,0,40);
	ofBeginShape();
	for (int i = 0; i < nCurveVertexes; i++){
		ofVertex(curveVertices[i].x, curveVertices[i].y);
	}
	ofEndShape(true);
	
	
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
	
	
	ofFill();
	ofSetHexColor(0xFF9933);
	ofBeginShape();
	ofVertex(x0,y0);
	ofBezierVertex(x1,y1,x2,y2,x3,y3);
	ofEndShape();
	
	
	ofEnableAlphaBlending();
	ofFill();
	ofSetColor(0,0,0,40);
	ofCircle(x0,y0,4);
	ofCircle(x1,y1,4);
	ofCircle(x2,y2,4);
	ofCircle(x3,y3,4);
	ofDisableAlphaBlending();
	
	
	// now using ofPath
	ofPushMatrix();
	ofTranslate( 0, 100, 0 );
	ofShape path;
	path.addVertex( x0, y0 );
	path.addBezierVertex( x1, y1, x2, y2, x3, y3 );
	ofSetHexColor(0xFF9933);
	path.tessellate();
	path.draw();

	ofEnableAlphaBlending();
	ofFill();
	ofSetColor(0,0,0,40);
	ofCircle(x0,y0,4);
	ofCircle(x1,y1,4);
	ofCircle(x2,y2,4);
	ofCircle(x3,y3,4);
	ofDisableAlphaBlending();
	
	ofPopMatrix();
	
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

