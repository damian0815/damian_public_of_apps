#include "testApp.h"
#include "ofxVectorMath.h"

//--------------------------------------------------------------
void testApp::setup(){
	
	ofSetFrameRate( 60.0f ) ;

	for ( int i=0; i<20; i++ )
		chain.addBone(20, ofRandom( 0, TWO_PI ), 0.05f);
	
	// anchor the root
	chain.getBone( 0 ).setWeightCentre( 1 );
	chain.setRootPos( ofxVec2f( ofGetWidth()/2 , ofGetHeight()/2 ) );
}

//--------------------------------------------------------------
void testApp::update(){

	chain.setTargetPos(ofxVec2f( mouseX, mouseY ));
	chain.solve( );

}

//--------------------------------------------------------------
void testApp::draw(){
	chain.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){


}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

