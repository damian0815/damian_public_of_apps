/*
 *  BufferedSerialDummy.h
 *  serialExample
 *
 *  Created by damian on 12/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "BufferedSerial.h"

#pragma once

class BufferedSerialDummy: public BufferedSerial
{
public:
	virtual void setup( ofSerial* _serial, int baud_rate ) {};
	virtual void shutdown() {};
	virtual void beginWrite() {};
	virtual bool writeBytes( unsigned char* buffer, int count ) { return count; }
	virtual void endWrite() {};
	
	
};
