#include "testApp.h"

int counter = 0;
//--------------------------------------------------------------
void testApp::setup(){
	//ofSetFrameRate(60); // if vertical sync is off, we can go a bit fast... this caps the framerate at 60fps.
	ofSetVerticalSync(false);

	
}

//--------------------------------------------------------------
void testApp::update(){

	ofSetWindowTitle(ofToString(ofGetFrameRate(), 0));

}

//--------------------------------------------------------------
void testApp::draw(){

	ofSetHexColor(0xFF0000);
	if( counter >= 3 )
		simpleShape.draw(400, 400);

	/*
	//--------------------------- circles
	//let's draw a circle:
	ofSetColor(255,130,0);
	float radius = 50 + 10 * sin(counter);
	ofFill();		// draw "filled shapes"
	ofCircle(100,400,radius);

	// now just an outline
	ofNoFill();
	ofSetHexColor(0xCCCCCC);
	ofCircle(100,400,80);

	// use the bitMap type
	// note, this can be slow on some graphics cards
	// because it is using glDrawPixels which varies in
	// speed from system to system.  try using ofTrueTypeFont
	// if this bitMap type slows you down.
	ofSetHexColor(0x000000);
	ofDrawBitmapString("circle", 75,500);


	//--------------------------- rectangles
	ofFill();
	for (int i = 0; i < 200; i++){
		ofSetColor((int)ofRandom(0,255),(int)ofRandom(0,255),(int)ofRandom(0,255));
		ofRect(ofRandom(250,350),ofRandom(350,450),ofRandom(10,20),ofRandom(10,20));
	}
	ofSetHexColor(0x000000);
	ofDrawBitmapString("rectangles", 275,500);

	//---------------------------  transparency
	ofSetHexColor(0x00FF33);
	ofRect(400,350,100,100);
	// alpha is usually turned off - for speed puposes.  let's turn it on!
	ofEnableAlphaBlending();
	ofSetColor(255,0,0,127);   // red, 50% transparent
	ofRect(450,430,100,33);
	ofSetColor(255,0,0,(int)(counter * 10.0f) % 255);   // red, variable transparent
	ofRect(450,370,100,33);
	ofDisableAlphaBlending();

	ofSetHexColor(0x000000);
	ofDrawBitmapString("transparency", 410,500);

	//---------------------------  lines
	// a bunch of red lines, make them smooth if the flag is set

	if (bSmooth){
		ofEnableSmoothing();
	}

	ofSetHexColor(0xFF0000);
	for (int i = 0; i < 20; i++){
		ofLine(600,300 + (i*5),800, 250 + (i*10));
	}

	if (bSmooth){
		ofDisableSmoothing();
	}

	ofSetHexColor(0x000000);
	ofDrawBitmapString("lines\npress 's' to toggle smoothness", 600,500);

	*/
}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	if (key == 's'){

	}
}

//--------------------------------------------------------------
void testApp::keyReleased  (int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

	if( button == 2)simpleShape.addCurveVertex(ofPoint(x-400, y-400, 0));
	else simpleShape.addVertex(ofPoint(x-400, y-400, 0));
	
	counter++;
	if(counter >=3) simpleShape.tessellate();
}


//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}
