#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
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
	
	int which_target;
	bool do_target_set;
	
	float z;
	float last_mx, last_my;
	float last_moved_mx, last_moved_my;

	void moveEye( float x, float y, float z);
	float heading, pitch, fov;
	ofxVec3f eye_pos;
	float move_speed, rotate_speed;
	bool rotate_eye;
	
	bool do_solve;
	bool do_walk;
	
	Cal3DModel model;
	IKCharacter character;
	
	vector<pair<string,string> > targets;
};

#endif

