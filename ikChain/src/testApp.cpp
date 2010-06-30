#include "testApp.h"
#include "ofxVectorMath.h"

//--------------------------------------------------------------
void testApp::setup(){

	last_mx = -1;
	last_moved_mx = -1;
	z = 0;
	
	draw_extended = false;
	
	heading = 0.0f;
	pitch = 0.0f;
	eye_pos.set( 0, 3.5, -10 );
	fov = 60.0f;
	move_speed = 0.15f;
	rotate_speed = 1.5f;
	do_solve = true;
	
	rotate_eye = false;
	
	
	ofSetFrameRate( 60.0f ) ;
	
	do_target_set = false;

	bool loaded_model = model.setup( "test", "man_good/man_good.xsf", "man_good/man_goodMan.xmf" );
//	bool loaded_model = model.setup( "doc", "doc/doctmp.csf", "doc/docmale.cmf" );
	if ( !loaded_model )
	{
		printf("couldn't load model\n");
		assert(false);
	}
	bool loaded_anim = model.loadAnimation( "man_good/man_goodSkeleton.001.xaf", "walk" );
	if ( !loaded_anim )
	{
		printf("couldn't load anim\n");
	}

	/*bool loaded_model = model.setup( "test", "man_w_anim/Man0.7.original_at_origin.xsf", "man_w_anim/Man.xmf" );
	if ( !loaded_model )
	{
		printf("couldn't load model\n");
		assert(false);
	}
	bool loaded_anim = model.loadAnimation( "man_w_anim/Action.xaf", "walk" );
	if ( !loaded_anim )
	{
		printf("couldn't load anim\n");
	}*/
	
	model.createInstance();
	
	do_walk = true;
	model.startAnimation( "walk" );
	
	// go from cal3d model to IKCharacter
	character.setup( model.getSkeleton() );
	
	which_target = 0;
	string root = "Root";
	string neck_joint = "Spine.1";
	targets.push_back( make_pair("Head",root) ); // head
	//targets.push_back( "Bone.001_R.004" ); // foot r
	//targets.push_back( "Bone.001_L.004" ); // foot l
	targets.push_back( make_pair("Hand.r", neck_joint) );
	targets.push_back( make_pair("Hand.l", neck_joint) );
	for ( int i=0; i<targets.size(); i++ )
	{
		character.enableTargetFor( targets[i].first, targets[i].second );
	}
}

//--------------------------------------------------------------
void testApp::update()
{
	model.updateAnimation( ofGetLastFrameTime() );

	// deal with looping motion
	CalVector loop_root_pos;
	if ( model.animationDidLoop( "walk", &loop_root_pos ) )
	{
		printf("-- loop\n");
		root_pos += (loop_root_pos)-model.getRootBonePosition();
	}
	
	character.pullFromModel();
	character.solve( 5 );
	character.pushToModel( do_solve );
	model.updateMesh();
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
	
	
	glPushMatrix();
	glTranslatef( 3.0f, 2.0f, 0.0f );
	character.draw( 1.0f, draw_extended );
	glPopMatrix();

	glPushMatrix();
	glTranslatef( root_pos.x, root_pos.y, root_pos.z );
	glTranslatef( -3.0f, 2.0f, 0 );
	glRotatef( 180, 0, 1, 0 );
	glRotatef( -90, 1, 0, 0 );
	// swap left handed to right handed
	glScalef( -1, 1, 1 );
	model.draw( true );
	glPopMatrix();
	
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
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	ofxVec3f pos;
	static int b_id = 0;
	int target_id;
	switch( key )
	{
		case '1':
			which_target = 0;
			last_mx = -1;
			break;
		case '2':
			which_target = 1;
			last_mx = -1;
			break;
		case '3':
			which_target = 2;
			last_mx = -1;
			break;
			
		case 'z':
			target_id = character.getTargetId( targets[(int)which_target].first );
			pos = character.getTarget( target_id );
			pos.z += 0.1f;
			character.setTarget( target_id, pos );
			break;
		case 'Z':
			target_id = character.getTargetId( targets[(int)which_target].first );
			pos = character.getTarget( target_id );
			pos.z -= 0.1f;
			character.setTarget( target_id, pos );
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
		
		case 'S':
			do_solve = !do_solve;
			break;
			
/*		case 'd':
			IKCharacter::debug_bone++;
			while ( IKCharacter::debug_bone >= model.getSkeleton()->getCoreSkeleton()->getNumBones() )
				IKCharacter::debug_bone--;
			break;
		case 'D':
			IKCharacter::debug_bone--;
			while( IKCharacter::debug_bone < 0 )
				IKCharacter::debug_bone++;
			break;*/
		
		case 'W':
			do_walk = !do_walk;
			if ( do_walk )
				model.startAnimation( "walk" );
			else
				model.stopAnimation( "walk" );
			break;
			
			
		case 'E':
			draw_extended = !draw_extended;
			break;
			
		case 'p':
			root_pos.set( 0,0,0 );
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
		int target_id = character.getTargetId( targets[(int)which_target].first );
		ofxVec3f pos = character.getTarget( target_id );
		pos.x += (x-last_mx)*0.02f;
		pos.y -= (y-last_my)*0.02f;
		character.setTarget( target_id, pos );
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

