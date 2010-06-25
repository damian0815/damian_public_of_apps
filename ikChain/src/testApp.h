#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "IKHumanoid.h"
#include "Cal3DModel.h"
#include "IKCharacter.h"

class testApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);

	
private:
	
	IKHumanoid human;
	IKHumanoid human2;
	IKHumanoid::Component which_target;
	bool do_target_set;
	
	float z;
	float last_mx, last_my;
	
	
	
	Cal3DModel model;
	Cal3DModel model2;

	
	IKCharacter character;
};

#endif
