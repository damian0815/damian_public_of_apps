/*
 *  BufferedSerial.h
 *  serialExample
 *
 *  Created by damian on 07/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include <assert.h>

extern void delayUs( unsigned long us );

class BufferedSerial
{
public:
	
	virtual void setup( ofSerial* _serial, int baud, float delay = 0.2f );
	virtual void shutdown();
	virtual void beginWrite();
	// blocks until all bytes are written + return true. returns false on error
	virtual bool writeBytes( unsigned char* buffer, int count );
	virtual void endWrite();
	
	void update( float elapsed );
	
	
protected:
	
	virtual bool writeBytes_real( unsigned char* buffer, int count );
	void pushOutPacket();
	
	int sent_this_block;

	ofSerial* serial;
	
	int baudrate;
	int bytes_written;
	float baud_timer;
	
	
	int delay_ms;
	const static int NUM_PACKETS = 64;
	struct Packet
	{
		int timer;
		unsigned char data[64];
		int count;
	};
	Packet packets[NUM_PACKETS];
	int read_packet;
	int write_packet;
	int ready_packets;
	
	
	
	
	
};
