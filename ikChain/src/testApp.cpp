#include "testApp.h"
#include "ofxVectorMath.h"

IKHumanoid::Cal3DModelMapping mapping;
IKHumanoid::Cal3DModelMapping mapping2;

//--------------------------------------------------------------
void testApp::setup(){

	last_mx = -1;
	last_my = 0;
	z = 0;
	
	ofSetFrameRate( 60.0f ) ;
	
	human.setup( 50.0f );
	
	do_target_set = false;
	which_target = IKHumanoid::C_SPINE;

	
 
	// map of model bone names for each component
	map< IKHumanoid::Component, pair<string,string> > bone_names;
	bone_names[IKHumanoid::C_SPINE] = make_pair( "spine_lo", "head" );
	bone_names[IKHumanoid::C_LEG_L] = make_pair( "hip l", "foot l" );
	bone_names[IKHumanoid::C_LEG_R] = make_pair( "hip r", "foot r" );
	bone_names[IKHumanoid::C_ARM_L] = make_pair( "shoulder l", "Hand.l" );
	bone_names[IKHumanoid::C_ARM_R] = make_pair( "shoulder r", "Hand.r" );
	string spine_arm_attach_name = "neck";
	string root_name = "root";
	
	
/*
 bool loaded_model = model.setup( "army", "armyman/armymantmp.xsf", "armyman/armymanCube.xmf" );
	map< IKHumanoid::Component, pair<string,string> > bone_names;
	bone_names[IKHumanoid::C_SPINE] = make_pair( "bodybottom", "head" );
	bone_names[IKHumanoid::C_LEG_L] = make_pair( "lhip", "lfoot2" );
	bone_names[IKHumanoid::C_LEG_R] = make_pair( "rhip", "rfoot2" );
	bone_names[IKHumanoid::C_ARM_L] = make_pair( "lshoulder", "lhand3" );
	bone_names[IKHumanoid::C_ARM_R] = make_pair( "rshoulder", "rhand3" );
	string spine_arm_attach_name = "neck";
	string root_name = "bodybottom";

*/
 
	bool loaded_model = model.setup( "test", "man_good/man_goodtmp.csf", "man_good/man_goodMan.cmf" );
	if ( !loaded_model )
	{
		printf("couldn't load model");
		assert(false);
	}
	model2.setup( "test2", "man_good/man_goodtmp.csf", "man_good/man_goodMan.cmf" );
	human.setup( );
	human2.setup( );
	//human.fromCal3DModel( model, 50.0f );
	
	// go from cal3d model to IKHumanoid
	// construct the mapping
	mapping.setup(model.getSkeleton()->getCoreSkeleton(), bone_names, root_name, spine_arm_attach_name );
	human.setupFromCal3DModel( &model, mapping );

	mapping2.setup(model2.getSkeleton()->getCoreSkeleton(), bone_names, root_name, spine_arm_attach_name );
	human2.setupFromCal3DModel( &model2, mapping );
	
	
}

//--------------------------------------------------------------
void testApp::update()
{

	human.resetToRest();
	//human.solve( 10 );
	
	//human2.resetToRest();
	
	human.updateCal3DModel();
	
	//human2.setupFromCal3DModel( &model, mapping );
	
	model.updateMesh();
}

//--------------------------------------------------------------
void testApp::draw(){
	glPushMatrix();
	glTranslatef( ofGetWidth()/2, ofGetHeight()/4, 0 );
	glRotatef( 10*sinf( ofGetElapsedTimef()/3), 0, 1, 0 );
	glRotatef( 10*sinf( ofGetElapsedTimef()/2.5), 1, 0, 0 );
	glScalef( 50, 50, 50 );
	human.draw( 0,0  );
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef( ofGetWidth()/3, ofGetHeight()/4, 0 );
	glRotatef( 20*sinf( ofGetElapsedTimef()/3), 0, 1, 0 );
	glScalef( 50, 50, 50 );
	human2.draw( 0,0 );
	glPopMatrix();

	glPushMatrix();
	glRotatef( 20*sinf( ofGetElapsedTimef()/3), 0, 1, 0 );
	glRotatef( 20*sinf( ofGetElapsedTimef()/2.5), 1, 0, 0 );
	model.draw( 30.0f );
	glPopMatrix();
	
	
	glPushMatrix();
	glTranslatef( ofGetWidth()/3, 3*ofGetHeight()/4, 0 );
	glRotatef( 20*sinf( ofGetElapsedTimef()/3), 0, 1, 0 );
	glRotatef( 20*sinf( ofGetElapsedTimef()/2.5), 1, 0, 0 );
	model.drawBones( 30.0f );
	glPopMatrix();
	glPushMatrix();
	glTranslatef( 2*ofGetWidth()/3, 3*ofGetHeight()/4, 0 );
	glRotatef( 20*sinf( ofGetElapsedTimef()/3), 0, 1, 0 );
	glRotatef( 20*sinf( ofGetElapsedTimef()/2.5), 1, 0, 0 );
	model2.drawBones( 30.0f );
	glPopMatrix();
	
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	ofxVec3f pos;
	static int b_id = 0;
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
			
		case 'b':
			b_id++;
			break;
		case 'B':
			b_id--;
			break;
		case 'x':
			model.rotateBoneX(b_id, 0.05);
			break;
		case 'X':
			model.rotateBoneX(b_id, -0.05);
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
		pos.x += (x-last_mx)*0.02f;
		pos.y += (y-last_my)*0.02f;
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

