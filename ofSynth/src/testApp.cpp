#include "testApp.h"



//--------------------------------------------------------------
void testApp::setup(){

	ofBackground(255,255,255);

	// 2 output channels,
	// 0 input channels
	// 22050 samples per second
	// 256 samples per buffer
	// 4 num buffers (latency)

	sampleRate 			= 44100;
	volume				= 0.1f;
	lAudio = new float[256];
	rAudio = new float[256];
	
	
	ofSoundStreamSetup(2,0, sampleRate,256, 4);

	ofSetFrameRate(60);

	
	// concert A
	testToneA.setFrequency( 440.0f );
	// connect the test tone to the mixer
	mixer.addInputFrom( &testToneA );
	
	mixer.setVolume( &testToneA, volume );
	
	// concert E
	testToneE.setFrequency( 329.6f );
	// connect tone to volume
	testVolume.addInputFrom( &testToneE );
	// connect volume to mixer
	mixer.addInputFrom( &testVolume );
	mixer.setVolume( &testVolume, 1.0f );

	// connect mixer to passthrough
	passthrough.addInputFrom( &mixer );

	
	ofSoundStreamAddSoundSource( &passthrough );
	

	
}


//--------------------------------------------------------------
void testApp::update(){

	const ofSoundBuffer& output = passthrough.getBuffer();
	output.copyChannel( 0, lAudio );
	output.copyChannel( 1, rAudio );
	
	if ( ofGetFrameNum() == 30 )
	{
		ofLog( OF_LOG_WARNING, "adding next" );
		// concert E
		testToneE2.setFrequency( 332.6f );
		// connect volume to mixer
		mixer.addInputFrom( &testToneE2 );
		mixer.setVolume( &testToneE2, 1.0f );
	}		
}

//--------------------------------------------------------------
void testApp::draw(){


	// draw the left:
	ofSetHexColor(0x333333);
	ofRect(100,100,256,200);
	ofSetHexColor(0xFFFFFF);
	for (int i = 0; i < 256; i++){
		ofLine(100+i,200,100+i,200+lAudio[i]*100.0f);
	}

	// draw the right:
	ofSetHexColor(0x333333);
	ofRect(600,100,256,200);
	ofSetHexColor(0xFFFFFF);
	for (int i = 0; i < 256; i++){
		ofLine(600+i,200,600+i,200+rAudio[i]*100.0f);
	}

	ofSetHexColor(0x333333);
	char reportString[255];
	sprintf(reportString, "volume: (%6.3f) modify with -/+ keys\nvolume: (%6.3f) modify with up/down keys", volume, testVolume.getVolume() );
	//if (!bNoise) sprintf(reportString, "%s (%fhz)", reportString, targetFrequency);

	ofDrawBitmapString(reportString,80,380);

}


//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	if (key == '-'){
		volume -= 0.05;
		volume = MAX(volume, 0);
		mixer.setVolume( &testToneA, volume );
	} else if (key == '+'){
		volume += 0.05;
		volume = MIN(volume, 1);
		mixer.setVolume( &testToneA, volume );
	}
	
	if ( key == OF_KEY_UP )
	{
		testVolume.adjustVolume( +0.25f );
	}
	else if ( key == OF_KEY_DOWN )
	{
		testVolume.adjustVolume( -0.25f );
	}
}

//--------------------------------------------------------------
void testApp::keyReleased  (int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
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

