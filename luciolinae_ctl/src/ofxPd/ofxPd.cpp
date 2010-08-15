/*
 *  ofxPd.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 12/08/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "ofxPd.h"
#include <assert.h>

extern "C"
{
#include "m_pd.h"
#include "m_imp.h"
#include "s_main.h"
#include "s_stuff.h"

extern void sched_audio_callbackfn(void);
};


// number of frames per section in the ring buffer
const static int BUFFER_SIZE_PER_CHANNEL = 256;
const static int NUM_BUFFERS = 8;

void ofxPd::setup( string _lib_dir )
{
	lib_dir = ofToDataPath(_lib_dir);
	num_channels = 2;
	ring_buffer.setup( BUFFER_SIZE_PER_CHANNEL*num_channels, NUM_BUFFERS );
}

void ofxPd::addOpenFile( string file_path )
{
	open_files.push_back( ofToDataPath( file_path ) );
}

void ofxPd::addSearchPath( string path )
{
	search_path.push_back( ofToDataPath( path ) );
}




void ofxPd::start()
{
	this->startThread();
}


void ofxPd::stop()
{
	if ( this->isThreadRunning() )
		sys_exit();
}


void ofxPd::threadedFunction()
{
	
	// concatenate string vectors to single strings
	string externs_cat, open_files_cat, search_path_cat;
	for ( int i=0; i<externs.size(); i++ )
	{
		if ( i > 0 )
			externs_cat += ":";
		externs_cat += externs[i];
	}
	for ( int i=0; i<open_files.size(); i++ )
	{
		if ( i > 0 )
			open_files_cat += ":";
		open_files_cat += open_files[i];
	}
	for ( int i=0; i<search_path.size(); i++ )
	{
		if ( i > 0 )
			search_path_cat += ":";
		search_path_cat += search_path[i];
	}
	
	int sound_rate = 44100;
	int block_size = 64;
	int n_out_channels = num_channels;
	int n_in_channels = 0;
	sys_main( lib_dir.c_str(), externs_cat.c_str(), open_files_cat.c_str(), search_path_cat.c_str(),
			 sound_rate, block_size, n_out_channels, n_in_channels );

}

void ofxPd::audioRequested( float* output, int bufferSize, int nChannels )
{
	assert( bufferSize*nChannels == ring_buffer.getBufferFrames() );
	// pull some sound from the ring buffer
	float* buffer = ring_buffer.getNextBufferToReadFrom();
	if ( buffer == NULL )
	{
		printf("underrun\n");
		// send 0
		memset( output, 0, sizeof(float)*bufferSize*nChannels);
	}
	else
	{
		memcpy( output, buffer, sizeof(float)*bufferSize*nChannels );
	}
}

bool ofxPd::isReady()
{
	return isThreadRunning() && sys_hasstarted;
}

void ofxPd::update()
{
	if ( !isReady() )
	{
		printf("pd is not ready\n");
		return;
	}
	else
		printf("pd is ready\n");
	while ( !ring_buffer.isFull() )
	{
		// fill buffer
		static float* buffer = NULL;
		if ( buffer == NULL )
			buffer = new float[BUFFER_SIZE_PER_CHANNEL*num_channels];
		
		// how many times to tick Pd
		int times = BUFFER_SIZE_PER_CHANNEL/sys_schedblocksize;
		
		// go
		for ( int i=0; i<times; i++ )
		{
			// do one DSP block
			sys_lock();
			sched_audio_callbackfn();
			sys_unlock();
			
			// This should cover sys_noutchannels channels. Turns non-interleaved into 
			// interleaved audio.
			for (int n = 0; n < sys_schedblocksize; n++) {
				for(int ch = 0; ch < sys_noutchannels; ch++) {
					t_sample fsample = CLAMP(sys_soundout[n+sys_schedblocksize*ch],-1,1);
					buffer[(n*sys_noutchannels+ch) + // offset in iteration
						   i*sys_schedblocksize*sys_noutchannels] // iteration starting address
					= fsample;
				}        
			}
			
			// After loading the samples, we need to clear them for the next iteration
			memset(sys_soundout, 0, sizeof(t_sample)*sys_noutchannels*sys_schedblocksize);        
		}
		
		// push to ring buffer
		ring_buffer.writeToNextBuffer( buffer );
	}
}








AudioRingBuffer::AudioRingBuffer()
{
	num_bufs = 0;
	ready = 0;
	pthread_mutex_init(&mutex, NULL); 
}

void AudioRingBuffer::setup(int buffsize, int _num_bufs )
{
	num_bufs = _num_bufs;
	buffers = new float*[num_bufs];
	for ( int i=0; i<num_bufs; i++ )
	{
		buffers[i] = new float[buffsize];
	}
	buf_size_bytes = buffsize*sizeof(float);
	current_read = 0;
	current_write = 0;
}

AudioRingBuffer::~AudioRingBuffer()
{
	if ( num_bufs > 0 )
	{
		for ( int i=0; i<num_bufs; i++ )
		{
			delete[] buffers[i];
		}
		delete[] buffers;
	}
	num_bufs = 0;
}


float* AudioRingBuffer::getNextBufferToReadFrom()
{
	if ( isEmpty() )
	{
		fprintf(stderr,"AudioRingBuffer()::getNextBufferToReadFrom(): buffer under-run\n");
		return NULL;
	}
	else printf("read: ready %i\n", ready );
	
	lock();
	int next = current_read++;
	if ( current_read >= num_bufs )
		current_read = 0;
	
	ready--;
	float* ret = buffers[next];
	unlock();
	
	return buffers[next];
}

void AudioRingBuffer::writeToNextBuffer( float* data )
{
	if ( isFull() )
	{
		fprintf(stderr,"AudioRingBuffer()::writeToNextBuffer(): buffer would overflow, not writing\n");
		return;
	}
	else printf("write: ready %i\n", ready );
	lock();
	memcpy( buffers[current_write], data, buf_size_bytes );
	ready++;
	current_write++;
	if ( current_write >= num_bufs )
		current_write = 0;
	unlock();
}
