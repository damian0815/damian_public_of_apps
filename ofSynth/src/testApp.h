#ifndef _TEST_APP
#define _TEST_APP


#include "ofMain.h"
#include "ofSynthMixer.h"
#include "testToneSynthUnit.h"

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

		void audioRequested 	(float * input, int bufferSize, int nChannels);

		int		sampleRate;
		float 	* lAudio;
		float   * rAudio;
	float volume;
/*	float pan;
	bool bNoise;
	float targetFrequency;*/

	
	testToneSynthUnit testToneA;
	testToneSynthUnit testToneE;
	ofSynthMixer mixer;
	
};

#endif
