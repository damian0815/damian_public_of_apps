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
	
	
	human.setup( 50.0f );
	
	do_target_set = false;
	which_target = IKHumanoid::C_SPINE;
}

//--------------------------------------------------------------
void testApp::update(){

	chain.solve( );

	human.solve();
}

//--------------------------------------------------------------
void testApp::draw(){
	chain.draw();
	
	human.draw( ofGetWidth()/2, ofGetHeight()/2 );
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	switch( key )
	{
		case '1':
			which_target = IKHumanoid::C_SPINE;
			break;
		case '2':
			which_target = IKHumanoid::C_ARM_L;
			break;
		case '3':
			which_target = IKHumanoid::C_ARM_R;
			break;
		case '4':
			which_target = IKHumanoid::C_LEG_L;
			break;
		case '5':
			which_target = IKHumanoid::C_LEG_R;
			break;
		default:
			break;
	}

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){


}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
	human.setTargetPos( which_target, ofxVec2f( x-ofGetWidth()/2, y-ofGetHeight()/2 ) );

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	human.setTargetPos( which_target, ofxVec2f( x-ofGetWidth()/2, y-ofGetHeight()/2 ) );

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

