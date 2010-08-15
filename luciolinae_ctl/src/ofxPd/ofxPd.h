/*
 *  ofxPd.h
 *  luciolinae_ctl
 *
 *  Created by damian on 12/08/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "ofMain.h"
#include "ofxThread.h"


class AudioRingBuffer
{
	public:

		AudioRingBuffer();

		void setup(int buffsize, int _num_bufs );

		~AudioRingBuffer();

		float* getNextBufferToReadFrom();
		
		void writeToNextBuffer( float* data );
	
		bool isEmpty() { return ready == 0; }
		bool isFull() { return num_bufs == ready; }
	
		int getBufferFrames() { return buf_size_bytes/sizeof(float); }
	
		void lock() { pthread_mutex_lock(&mutex); }
		void unlock() { pthread_mutex_unlock( &mutex); }

	private:
		float** buffers;

		pthread_mutex_t  mutex;

		int num_bufs;
		int buf_size_bytes;
		int ready;
		int current_read;
		int current_write;

};

class ofxPd;
class ofxPdUpdateThread: public ofxThread
{
public:
	ofxPdUpdateThread( ofxPd* _target ) { target = _target; should_stop = false; stopped = false; }
	
	// stop the thread, now
	void stopNow() { should_stop = true; while ( !stopped ) usleep( 1000 ); }
	
private:
	void threadedFunction();
	
	ofxPd* target;
	bool should_stop;
	bool stopped;
};

class ofxPd : public ofxThread
{
public:	
	ofxPd(): update_thread( this ) { ready = false; }
	
	/// lib_dir is the directory in which to look for pd files
	void setup( string lib_dir );

	/// fill ring buffer if necessary
	void update();
	
	/// add the given file to the list to be opened on startup
	void addOpenFile( string file_path );
	
	/// add the given path to the search path
	void addSearchPath( string search_path );
	
	
	/// start pd
	void start();
	/// stop pd
	void stop();
	
	/// callback for audio rendering
	void audioRequested( float* output, int bufferSize, int nChannels );
	
	/// true if pd has been started properly and is ready
	bool isReady();
	
private:

	// the thing to run in a thread
	void threadedFunction();
	
	string lib_dir;
	int num_channels;
	
	vector <string> externs;
	vector <string> search_path;
	vector <string> open_files;
	
	AudioRingBuffer ring_buffer;
	bool ready;
	
	ofxPdUpdateThread update_thread;
};

