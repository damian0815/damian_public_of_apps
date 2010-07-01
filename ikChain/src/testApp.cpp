#include "testApp.h"
#include "ofxVectorMath.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofSetFrameRate( 60.0f ) ;

	last_mx = -1;
	last_moved_mx = -1;
	
	heading = 0.0f;
	pitch = 0.0f;
	eye_pos.set( 0, 3.5, -10 );
	fov = 60.0f;
	move_speed = 0.15f;
	rotate_speed = 1.5f;
	
	rotate_eye = false;
	
	
	z = 0;
	
	tagger.setup();
	
}

//--------------------------------------------------------------
void testApp::update()
{
	tagger.update( ofGetLastFrameTime() );
}

//--------------------------------------------------------------
void testApp::draw(){
	
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

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
	
	tagger.draw();
	
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
	
	// now draw some debugging stuff
	ofSetColor( 0x000000 );
	ofDrawBitmapString( "Keys:\n"
					   "1-3 select targets    mouse drag target pos     z/Z move target z\n"
					   "; toggle mouse move eye rotation    ae,o.j (dvorak==qwerty adwsec) move eye pos\n"
					   "shift-W toggle walk cycle           p reset root pos to 0,0,0   \n"
					   "shift-S toggle adaptive ik solve (default enabled)      shift-E draw extended skeleton", 10, 23 );
	ofxVec3f tt = tagger.getTagArmTarget();
	char buf[256];
	sprintf(buf, "tag arm target now: %f %f %f\n", tt.x, tt.y, tt.z );
	ofDrawBitmapString( buf, 10, 101 );
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	ofxVec3f pos;
	static int b_id = 0;
	int target_id;
	switch( key )
	{
		case ';':
			rotate_eye = !rotate_eye;
			break;
		case ':':
			printf("eye at %f %f %f heading %f pitch %f\n", eye_pos.x, eye_pos.y, eye_pos.z, heading, pitch );
			break;
			
		case 'P':
			tagger.setRootPosition( ofxVec3f(0,0,0) );
			break;
			
		case 'z':
			z += 0.1f;
			break;
		case 'Z':
			z -= 0.1f;
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
	
	ofxVec3f tt = tagger.getTagArmTarget();
	tt += ofxVec3f( 4*(float(x-last_mx)/ofGetWidth()), 4*(float(y-last_my)/ofGetHeight()), 0 );
	tt.z = z;
	tagger.setTagArmTarget( tt );
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

