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

	model.setup( "test", "man_good/man_goodtmp.xsf" );
	model.dumpSkeleton();
	human2.setup( 50.0f );
	human.fromCal3DModel( model, 50.0f );
	
}

//--------------------------------------------------------------
void testApp::update(){

	human.resetToRest();
	human.solve( 10 );
	
	//human2.resetToRest();
}

//--------------------------------------------------------------
void testApp::draw(){
	human.draw( ofGetWidth()/2, ofGetHeight()/2 );
	human2.draw( ofGetWidth()/4, ofGetHeight()/2 );
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	ofxVec3f pos;
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
			pos = human.getTargetPos( which_target );
			pos.z += 1.0f;
			human.setTargetPos( which_target, pos );
			break;
		case 'Z':
			pos = human.getTargetPos( which_target );
			pos.z -= 1.0f;
			human.setTargetPos( which_target, pos );
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
	
	if ( last_mx > 0 )
	{
		ofxVec3f pos = human.getTargetPos( which_target );
		pos.x += (x-last_mx);
		pos.y -= (y-last_my);
		human.setTargetPos( which_target, pos );
	}
	last_mx = x;
	last_my = y;
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
	 last_mx = x;
	 last_my = y;
	

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

	last_mx = -1;
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

