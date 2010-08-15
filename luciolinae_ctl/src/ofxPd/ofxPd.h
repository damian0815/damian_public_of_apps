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

		AudioRingBuffer()
		{
			num_bufs = 0;
		}

		void setup(int buffsize, int num_bufs )
		{
			for ( int i=0; i<num_bufs; i++ )
			{
				buffers[i] = new float[buffsize];
			}
		}

		~AudioRingBuffer()
		{
			for ( int i=0; i<num_bufs; i++ )
			{
				delete[] buffers[i];
			}
			num_bufs = 0;
		}



		float* getNextBuffer()
		{
			if ( ready == 0 )
			{
				fprintf(stderr,"AudioRingBuffer()::buffer under-run\n");
				return NULL;
			}

			int next = current++;
			ready--;
			if ( current >= num_bufs )
				current = 0;

			return buffers[next];
		}


	private:
		float** buffers;

		int num_bufs;
		int ready;
		int current_read;
		int current_write;

};

class ofxPd : public ofxThread
{
public:	
	
	/// lib_dir is the directory in which to look for pd files
	void setup( string lib_dir );
	
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
	
	vector <string> externs;
	vector <string> search_path;
	vector <string> open_files;
	
	
};

