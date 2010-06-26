#include "testApp.h"
#include "ofxVectorMath.h"

//--------------------------------------------------------------
void testApp::setup(){

	last_mx = -1;
	last_moved_mx = -1;
	z = 0;
	
	
	heading = 180.0f;
	pitch = 0.0f;
	eye_pos.set( 0, 3.5, 5 );
	fov = 60.0f;
	move_speed = 0.15f;
	rotate_speed = 1.5f;
	do_solve = true;
	
	rotate_eye = false;
	
	
	ofSetFrameRate( 60.0f ) ;
	
	do_target_set = false;
	which_target = IKHumanoid::C_SPINE;

	bool loaded_model = model.setup( "test", "man_good/man_goodtmp.csf", "man_good/man_goodMan.cmf" );
	if ( !loaded_model )
	{
		printf("couldn't load model");
		assert(false);
	}
	// go from cal3d model to IKCharacter
	character.setup( model.getSkeleton() );
	
}

//--------------------------------------------------------------
void testApp::update()
{
	model.resetToRest();
	character.pullWorldPositions();
	character.solve( 10 );
	character.pushWorldPositions( do_solve );
	model.updateMesh();
}

//--------------------------------------------------------------
void testApp::draw(){
	
	

	// load proper 3d world
	
	float w = 10;
	float h = 10;
	
	float halfFov, theTan, screenFov, aspect;
	screenFov 		= fov;
	
	float eyeX 		= /*(float)w / 2.0;*/eye_pos.x;
	float eyeY 		= /*(float)w / 2.0;*/eye_pos.y;
	halfFov 		= PI * screenFov / 360.0;
	theTan 			= tanf(halfFov);
	float dist 		= (h/2) / theTan;
	float nearDist 	= dist / 1000.0;	// near / far clip plane
	float farDist 	= dist * 10000.0;
	aspect 			= (float)w/(float)h;
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(screenFov, aspect, nearDist, farDist);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// centre
	ofxVec3f d_centre(0, 0, dist);
	d_centre.rotate( heading, ofxVec3f(0, 1, 0) );
	d_centre.rotate( pitch, ofxVec3f(1, 0, 0) );
	ofxVec3f centre = eye_pos + d_centre;
	gluLookAt(eyeX, eyeY, eye_pos.z, centre.x, centre.y, centre.z, 0.0, 1.0, 0.0);
	
	//glScalef( -1, -1, 1 );
	
	// draw ground plane
	glBegin( GL_LINES );
	glColor3f( 0.6f, 0.6f, 1.0f );
	for ( int i=0; i<20; i++ )
	{
		glVertex3f( -10, 0, i-10 );
		glVertex3f( 10, 0, i-10 );
	}
	for ( int i=0; i<20; i++ )
	{
		glVertex3f( i-10, 0, -10 );
		glVertex3f( i-10, 0, 10 );
	}
	glEnd();
	
	
	glColor3f( 0, 0, 0 );
	
	
	glPushMatrix();
	glTranslatef( 3.0f, 2.0f, 0.0f );
	character.draw( 1.0f );
	glPopMatrix();

	glPushMatrix();
	glTranslatef( -3.0f, 2.0f, 0 );
	glRotatef( 180, 0, 1, 0 );
	glRotatef( -90, 1, 0, 0 );
	glScalef( -1, 1, 1 );
	// swap left handed to right handed
	model.draw(  );
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
			pos = character.getTarget( character.getLeafId( (int)which_target) );
			pos.z += 0.1f;
			character.setTarget( character.getLeafId( (int)which_target), pos );
			break;
		case 'Z':
			pos = character.getTarget( character.getLeafId( (int)which_target) );
			pos.z -= 0.1f;
			character.setTarget( character.getLeafId( (int)which_target), pos );
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
			
		case ';':
			rotate_eye = !rotate_eye;
			break;
		
		case 's':
			do_solve = !do_solve;
			break;
			
		case 'd':
			IKCharacter::debug_bone++;
			while ( IKCharacter::debug_bone >= model.getSkeleton()->getCoreSkeleton()->getNumBones() )
				IKCharacter::debug_bone--;
			break;
		case 'D':
			IKCharacter::debug_bone--;
			while( IKCharacter::debug_bone < 0 )
				IKCharacter::debug_bone++;
			break;
		
			
			
		default:
			break;
			
			
	}
	
	if ( key == ',' )
		moveEye( 0, 0, move_speed );
	else if ( key == 'o' )
		moveEye( 0, 0, -move_speed );
	else if ( key == 'a' )
		moveEye( move_speed, 0, 0 );
	else if ( key == 'e' )
		moveEye( -move_speed, 0, 0 );
	else if ( key == '.' )
		//moveEye( 0, move_speed, 0 );
		eye_pos.y += move_speed;
	else if ( key == 'j' )
		eye_pos.y -= move_speed;
		//moveEye( 0, -move_speed, 0 );
	
}

void testApp::moveEye( float x, float y, float z)
{
	ofxVec3f relative( x,y,z );
	relative.rotate( heading, ofxVec3f(0, 1, 0) );
	relative.rotate( pitch, ofxVec3f( 1, 0, 0 ) );
	eye_pos += relative;
	
	//camera_a.move( relative );
}



//--------------------------------------------------------------
void testApp::keyReleased(int key){
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	if ( last_moved_mx != -1 && rotate_eye )
	{
		heading += 180*rotate_speed*float(last_moved_mx-x)/ofGetWidth();
		pitch += 90*rotate_speed*float(last_moved_my-y)/ofGetHeight();
	}
	last_moved_mx = x;
	last_moved_my = y;

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
	if ( last_mx > 0 )
	{
		ofxVec3f pos = character.getTarget( character.getLeafId( (int)which_target) );
		pos.x += (x-last_mx)*0.02f;
		pos.y -= (y-last_my)*0.02f;
		character.setTarget( character.getLeafId( (int)which_target), pos );
	}
	last_mx = x;
	last_my = y;
	last_moved_mx = -1;
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
	last_mx = x;
	last_my = y;
	last_moved_mx = -1;
	

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

	last_mx = -1;
	last_moved_mx = x;
	last_moved_my = y;

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

