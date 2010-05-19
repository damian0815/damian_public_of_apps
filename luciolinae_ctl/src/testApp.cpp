#include "testApp.h"
#include "AnimSweep.h"
#include "AnimPositionCalibrate.h"
#include "AnimDelaunay.h"
#include "AnimStateMachine.h"


//--------------------------------------------------------------
void testApp::setup(){	 

	ofSetVerticalSync(true);
	ofBackground(20,20,20);	
	ofEnableAlphaBlending();
	ofSetFrameRate( 30 );
	
	ofSetLogLevel( OF_LOG_WARNING );
	
	serial.enumerateDevices();
			
	
	//----------------------------------- note:
	// < this should be set
	// to whatever com port
	// your serial device is
	// connected to.
	// (ie, COM4 on a- pc, /dev/tty.... on linux, /dev/tty... on a mac)
	// arduino users check in arduino app....

	//serial.setup("COM4");  						  // windows example
	//serial.setup("/dev/tty.usbserial-A5001aub",9600); // mac osx example
	//serial.setup("/dev/ttyUSB0", 9600);			  //linux example
	static const char* SERIAL_PORT = "/dev/tty.usbserial-FTT8R2AA";
	if ( serial.setup(SERIAL_PORT,9600) )
		buffered_serial = new BufferedSerial();
	else
	{
		printf("couldn't open serial %s, making dummy\n", SERIAL_PORT );
		buffered_serial = new BufferedSerialDummy();
	}
	buffered_serial->setup( &serial );

	
	ofxXmlSettings data;
	if ( data.loadFile( "settings.xml" ) )
		lights.setup( buffered_serial, data );
	else
		lights.setup( buffered_serial );

	AnimationFactory::useLights( &lights );
	anim_switcher.addAnim( AnimSweep::NAME );
	anim_switcher.addAnim( AnimDelaunay::NAME );
	anim_switcher.addAnim( AnimPositionCalibrate::NAME );
	anim_switcher.addAnim( AnimStateMachine::NAME );
	
	current_anim = anim_switcher.currentAnim();
}

//--------------------------------------------------------------
void testApp::update(){
	// update lights
	lights.update( ofGetLastFrameTime() );
	
	
	// update animation
	current_anim->update( ofGetLastFrameTime() );
	
	//lights.illuminateCircularArea( float(mouseX)/ofGetWidth(), float(mouseY)/ofGetHeight(), illArea );

	// send light levels
	lights.flush();
	
	/*
	// test
	static unsigned int current = 2048<<4;	
	static float current_float = 2048;
	unsigned int elapsed = ofGetLastFrameTime()*(1<<10);
	float decay_float = 0.5f;
	unsigned int decay_fixed = decay_float*(1<<4);
	// floating point math: current = current-current*elapsed*decay_pct
	// this would mean that every second, current would lose decay_pct of its value
	//
	// fixed point math: current is *2^0, elapsed is *2^-10 (millis->seconds), decay is *2^-4
	// we have 32 bits of precision in an unsigned long int, so this is ok
	// and then shift to correct for multiplication
	// -10+-4 = -14
	unsigned int next;
	if ( current == 0 )
		next = current;
	else
		next = current - (((current*elapsed*decay_fixed)>>14)+1);
	
	// float to compare
	float float_factor = ofGetLastFrameTime()*decay_float;
	float next_float = current_float - current_float*float_factor;
	
	printf("elapsed %3u; %4u decays by %f/%u -> fixed: %4u, float: %f (factor %f)\n", elapsed, current>>4, decay_float, decay_fixed, next>>4, next_float, float_factor );
	current_float = next_float;
	current = next;
	 */
}

//--------------------------------------------------------------
void testApp::draw(){
	lights.draw();
	
	current_anim->draw();
}

void testApp::exit()
{
	printf("clearing lights\n");
	lights.clear();
	buffered_serial->shutdown();
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
	
	switch( key )
	{
		// panic button
		case '!':
			lights.clear( true );
			break;
		case 'p':
			lights.pulseAll( 0.25 );
			break;
		case 'W':
		{
			ofxXmlSettings data;
			lights.save( data );
			data.saveFile( "settings.xml" );
			break;
		}
		case '[':
			current_anim = anim_switcher.prevAnim();
			lights.clear();
			break;
		case ']':
			current_anim = anim_switcher.nextAnim();
			lights.clear();
			break;
		default:
			break;
			
	}
	current_anim->keyPressed( key );
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){ 
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	current_anim->mouseMoved( x, y );
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

