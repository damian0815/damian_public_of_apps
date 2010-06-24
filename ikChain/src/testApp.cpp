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

	bool loaded_model = model.setup( "test", "man_good/man_goodtmp.xsf", "man_good/man_goodMan.xmf" );
	if ( !loaded_model )
	{
		printf("couldn't load model");
		assert(false);
	}
	model.dumpSkeleton();
	human2.setup( 50.0f );
	human.fromCal3DModel( model, 50.0f );
	
	// map of model bone names for each component
	map< IKHumanoid::Component, pair<string,string> > bone_names;
	bone_names[IKHumanoid::C_SPINE] = make_pair( "spine_lo", "head" );
	bone_names[IKHumanoid::C_LEG_L] = make_pair( "hip l", "foot l" );
	bone_names[IKHumanoid::C_LEG_R] = make_pair( "hip r", "foot r" );
	bone_names[IKHumanoid::C_ARM_L] = make_pair( "shoulder l", "hand l" );
	bone_names[IKHumanoid::C_ARM_R] = make_pair( "shoulder r", "hand r" );
	string spine_arm_attach_name = "neck";
	// construct the mapping
	IKHumanoid::Cal3DModelMapping mapping;
	mapping.setup(model.getSkeleton()->getCoreSkeleton(), bone_names, spine_arm_attach_name );
										  
	// go from cal3d model to IKHumanoid
	human2.fromCal3DModel( model, mapping, 50.0f );
	
}

//--------------------------------------------------------------
void testApp::update()
{

	human.resetToRest();
	human.solve( 10 );
	
	//human2.resetToRest();
	
	model.updateMesh( );
}

//--------------------------------------------------------------
void testApp::draw(){
	human.draw( ofGetWidth()/2, ofGetHeight()/2 );
	human2.draw( ofGetWidth()/4, ofGetHeight()/2 );
	model.draw( 50.0f );
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

