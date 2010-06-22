#include "testApp.h"
#include "ofxVectorMath.h"

//--------------------------------------------------------------
void testApp::setup(){

	last_mx = -1;
	last_my = 0;
	z = 0;
	
	ofSetFrameRate( 60.0f ) ;
	
	human.setup( 50.0f );
	
	do_target_set = false;
	which_target = IKHumanoid::C_SPINE;
	
}

//--------------------------------------------------------------
void testApp::update(){

	if ( last_mx > 0 )
		human.setTargetPos( which_target, ofxVec3f( last_mx-ofGetWidth()/2, ofGetHeight()/2-last_my, z ) );
		
	human.resetToRest();
	human.solve( 10 );
}

//--------------------------------------------------------------
void testApp::draw(){
	human.draw( ofGetWidth()/2, ofGetHeight()/2 );
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	switch( key )
	{
		case '1':
			which_target = IKHumanoid::C_SPINE;
			last_mx = -1;
			break;
		case '2':
			which_target = IKHumanoid::C_ARM_L;
			last_mx = -1;
			break;
		case '3':
			which_target = IKHumanoid::C_ARM_R;
			last_mx = -1;
			break;
		case '4':
			which_target = IKHumanoid::C_LEG_L;
			last_mx = -1;
			break;
		case '5':
			which_target = IKHumanoid::C_LEG_R;
			last_mx = -1;
			break;
			
		case 'z':
			z += 1.0f;
			break;
		case 'Z':
			z -= 1.0f;
			break;
		case '6':
			ofSetFrameRate( 60.0f );
			break;
		case '7':
			ofSetFrameRate( 24.0f );
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
	
	human.setTargetPos( which_target, ofxVec3f( x-ofGetWidth()/2, ofGetHeight()/2-y, z ) );
	last_mx = x;
	last_my = y;

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	human.setTargetPos( which_target, ofxVec3f( x-ofGetWidth()/2, ofGetHeight()/2-y, z ) );
	last_mx = x;
	last_my = y;

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

