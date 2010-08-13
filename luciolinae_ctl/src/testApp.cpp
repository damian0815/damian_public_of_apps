#include "testApp.h"
#include "AnimSweep.h"
#include "AnimPositionCalibrate.h"
#include "AnimDelaunay.h"
#include "AnimStateMachine.h"
#include "AnimSeq.h"
#include "StateAnimIdle.h"


//--------------------------------------------------------------
void testApp::setup(){	 

	printf("testApp::setup()\n");

#ifndef NO_WINDOW
	ofSetVerticalSync(true);
	ofBackground(20,20,20);	
	ofEnableAlphaBlending();
	ofSetFrameRate( 30 );
#endif
	
#ifdef NO_WINDOW
	ofSetDataPathRoot( "data/" );
	ofSetFrameRate( 60 ); 
#endif
	
	ofSetLogLevel( OF_LOG_WARNING );
	
	serial.enumerateDevices();
#ifdef TARGET_LINUX
	static const char* SERIAL_PORT = "/dev/ttyUSB0";
#elif defined TARGET_OSX
	//static const char* SERIAL_PORT = "/dev/tty.usbserial-FTT8R2AA";
	//static const char* SERIAL_PORT = "/dev/tty.usbserial-0000103D";
	//static const char* SERIAL_PORT = "/dev/tty.usbserial-A4000R0L";
	static const char* SERIAL_PORT	 = "/dev/tty.usbserial-FTTEIQQY";
#endif
	static const int BAUDRATE = 19200;
	if ( serial.setup(SERIAL_PORT, BAUDRATE ) )
		buffered_serial = new BufferedSerial();
	else
	{
		printf("couldn't open serial %s, making dummy\n", SERIAL_PORT );
		buffered_serial = new BufferedSerialDummy();
	}
	buffered_serial->setup( &serial, BAUDRATE );

	// osc
	static const char* OSC_HOST = "localhost";
	static const int OSC_PORT = 10001;
	osc.setup( OSC_HOST, OSC_PORT );
	
	
	// load settings
	ofxXmlSettings data;
	if ( data.loadFile( "settings.xml" ) )
		lights.setup( buffered_serial, data );
	else
	{
		ofLog( OF_LOG_ERROR, "couldn't load data/settings.xml");
		lights.setup( buffered_serial );
	}

	AnimationFactory::useLights( &lights );
	anim_switcher.addAnim( AnimStateMachine::NAME );
	anim_switcher.addAnim( AnimSweep::NAME );
	anim_switcher.addAnim( AnimDelaunay::NAME );
	anim_switcher.addAnim( AnimPositionCalibrate::NAME );
	anim_switcher.addAnim( AnimSeq::NAME );

	current_anim = anim_switcher.goToAnim( AnimStateMachine::NAME );
	
	printf("starting pd...\n");
	
	ofSoundStreamSetup(2, 0, this, 44100, 256, 4 );
	pd.setup( "" );
	//pd.addOpenFile( "pd-test.pd" );
	pd.addOpenFile( "pdstuff/_main.pd" );
	pd.start();
	
	printf("testApp::setup() finished\n");
	
}

//--------------------------------------------------------------
void testApp::update(){
	buffered_serial->update( ofGetLastFrameTime() );
	breath.update( ofGetLastFrameTime() );
	
	// update delaunay pulses
	pulses.update( ofGetLastFrameTime() );
	

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
#ifndef NO_WINDOW
	lights.draw();
	current_anim->draw();
#endif
}

void testApp::exit()
{
	printf("clearing lights\n");
	lights.clear( true );
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
			lights.pulseAll( 1 );
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
		
		case 'b':
			lights.increaseSmallLightBrightnessFactor();
			break;
		case 'B':
			lights.decreaseSmallLightBrightnessFactor();
			break;
		
		case 'R':
			ofSeedRandom( 12345 );
			break;
		case 'r':
			StateAnimIdle::should_reset_random = true;
			
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


void testApp::audioRequested( float* input, int bufferSize, int nChannels )
{
	if ( pd.isThreadRunning() )
	{
		pd.audioRequested( input, bufferSize, nChannels );
	}
	else
	{
		memset( input, 0, sizeof( input ) );
	}
		
}
