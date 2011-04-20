#include "testApp.h"

void testApp::reset(){

	wasps.resize(5);
	for ( int i=0;i <wasps.size(); i++ )
	{
		wasps[i].setup( &input_history );
		wasps[i].setPosition( ofVec2f(ofRandom( ofGetWidth()*1.0f/4,  ofGetWidth()*3.0f/4 ), 
									  ofRandom( ofGetHeight()*1.0f/4, ofGetHeight()*3.0f/4) ) 
							 );
	}
	
}

//--------------------------------------------------------------
void testApp::setup(){	
	// register touch events
	ofxRegisterMultitouch(this);
	
	//iPhoneAlerts will be sent to this.
	ofxiPhoneAlerts.addListener(this);
	
	reset();
	
	ofEnableAlphaBlending();
	
	ofSetLogLevel(OF_LOG_VERBOSE);
}

//--------------------------------------------------------------
void testApp::update() {
	for ( int i=0; i<wasps.size(); i++ )
	{
		wasps[i].update();
	}
	
}

//--------------------------------------------------------------
void testApp::draw() {
	input_history.draw();

	ofBackground( 255, 255, 255 );
	for ( int i=0; i<wasps.size(); i++ )
	{
		wasps[i].draw();
	}
	
}

//--------------------------------------------------------------
void testApp::exit() {
	printf("exit()\n");
}

//--------------------------------------------------------------
void testApp::touchDown(int x, int y, int id){

	input_history.addTouch( ofGetElapsedTimef(), ofVec2f(x,y) );
}

//--------------------------------------------------------------
void testApp::touchMoved(int x, int y, int id){
	input_history.addTouch( ofGetElapsedTimef(), ofVec2f(x,y) );
}

//--------------------------------------------------------------
void testApp::touchUp(int x, int y, int id){
}

//--------------------------------------------------------------
void testApp::touchDoubleTap(int x, int y, int id){
	reset();
}

//--------------------------------------------------------------
void testApp::lostFocus() {
}

//--------------------------------------------------------------
void testApp::gotFocus() {
}

//--------------------------------------------------------------
void testApp::gotMemoryWarning() {
}

//--------------------------------------------------------------
void testApp::deviceOrientationChanged(int newOrientation){
	
}
