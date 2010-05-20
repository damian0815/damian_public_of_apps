#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "Lights.h"
#include "BufferedSerialDummy.h"
#include "Animation.h"
#include "Activators.h"
#include "DelaunayPulse.h"

class testApp : public ofBaseApp{
	
public:
	testApp() : pulses( &lights ) { printf("testApp constructor called\n"); };
	
	void setup();
	void update();
	void draw();
	void exit();
	
	void keyPressed(int key);
	void keyReleased(int key);
	
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	
	
	ofSerial	serial;
	BufferedSerial* buffered_serial;
		
	Lights lights;
	
	Animation* current_anim;
	AnimationSwitcher anim_switcher;
	DelaunayPulses pulses;
};


#endif	

