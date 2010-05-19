/*
 *  Lights.cpp
 *  serialExample
 *
 *  Created by damian on 07/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "Lights.h"
#include "ofxXmlSettings.h"
#include "NearestPointOnLineToPoint.h"
#include "LightsDelaunay.h"

static const int NUM_LIGHTS=16*2;


static const unsigned char FUNC_SET_EVERY = 0x06;
static const unsigned char FUNC_LATCH = 0x04;



Lights::~Lights()
{ 
	if ( delaunay ) delete delaunay; 
}



void Lights::setup( BufferedSerial* _serial )
{
	serial = _serial;
	num_boards = 0;
	
	lights.resize( NUM_LIGHTS );
	// square grid
	int grid_size = sqrtf( NUM_LIGHTS );
	for ( int i=0; i<NUM_LIGHTS; i++ )
	{
		float y = (float(i/grid_size)+0.5f+ofRandom(-0.5,0.5))/(grid_size+2);
		float x = (float(i%grid_size)+0.5f+ofRandom(-0.5,0.5))/grid_size;
		lights[i].setup( (i/16 + 1)<<4, i%16, x, y );
		// how many boards?
		if ( i%16==0 )
			num_boards++;
	}
	
	clear( true );
}

void Lights::setup( BufferedSerial* _serial, ofxXmlSettings& data )
{
	serial = _serial;
	
	data.pushTag("lights");
	num_boards = data.getValue("num_boards", 0 );
	int num_lights = data.getNumTags( "light" );
	lights.resize( num_lights );
	for ( int i=0; i<num_lights; i++ )
	{
		data.pushTag("light", i );
		lights[i].setup( data );
		data.popTag();
		// remember the big lights
		if ( lights[i].isBig() )
		{
			big_lights.push_back( i );
		}
	}
	std::sort( big_lights.begin(), big_lights.end() );
	data.popTag();
	
	clear(true);

}

void Lights::save( ofxXmlSettings& data )
{
	data.addTag("lights");
	data.pushTag( "lights" );
	data.addValue("num_boards", num_boards );
	for ( int i=0; i<lights.size(); i++ )
	{
		data.addTag("light" );
		data.pushTag("light", i );
		lights[i].save( data );
		data.popTag();
	}
	data.popTag();
	
	
}

void Lights::update( float elapsed )
{
	// update lights
	for ( int i=0;i<lights.size(); i++ )
		lights[i].update(elapsed);
	
	// then send serial again if necessary
	flush();
}

void Lights::flush()
{
	//printf("flush called\n");
	serial->beginWrite();
	vector<int> counts;
	// fill with 0
	counts.insert( counts.begin(), num_boards, 0 );
	// count up how many per board
	for ( int i=0; i<lights.size(); i++ )
	{
		if ( lights[i].needsSerial() && !lights[i].wantsPulse() )
			counts[(lights[i].getBoardId()>>4)-1]++;
	}
	for ( int i=0; i<num_boards; i++ )
	{
		// if more than 8, best to send all at once
		if /*( counts[i] > 8 )*/(false)
		{
//			printf("compiling every light level data for board %i\n", i);
			unsigned char total_board_data[24];
			for ( int j=0; j<lights.size(); j++ )
			{
				// find this board's lights
				if ( lights[j].getBoardId()==(i+1)<<4 )
				{
					// add data to array
					int light_id = lights[j].getLightId();
					int base_index = (light_id*3>>1);
					unsigned int brightness = min(4095,max(0,int(lights[j].getBrightness()*4096)));
					//printf("  adding light %02x:%02x brightness %03x\n", lights[j].getBoardId(), lights[j].getLightId(), brightness );
					if ( light_id%2 == 0 )
					{
						// starts clean
						total_board_data[base_index  ] = brightness>>4;
						total_board_data[base_index+1] = (unsigned char)(brightness<<4) | (total_board_data[base_index+1]&0x0f);
					}
					else
					{
						// starts in the middle of the byte
						total_board_data[base_index  ] = (total_board_data[base_index]&0xf0) | (brightness>>8); 
						total_board_data[base_index+1] = brightness & 0xff;
					}
				}
			}
			//printf("sending every light level data\n");
			// now send total_board_data
			sendEveryLightLevel( (i+1)<<4, total_board_data );
		}
		else
		{
//			printf("sending levels for board %i individually\n", i );
			// send lights for this board individually
			for ( int j=0; j<lights.size(); j++ )
			{
				// find this board's lights
				if ( (lights[j].getBoardId()==(i+1)<<4) && lights[j].needsSerial() && !lights[j].wantsPulse() )
				{
					int brightness = lights[j].getBrightness()*4096;
					sendLightLevel( lights[j].getBoardId(), lights[j].getLightId(), brightness );
				}
			}
		}
	}
	
	// latch in the data
	latch();

	// send pulses
	for ( int j=0; j<lights.size(); j++ )
	{
		// find this board's lights
		if ( lights[j].needsSerial() && lights[j].wantsPulse() )
		{
			int brightness = lights[j].getBrightness()*4096;
			float decay_float = lights[j].getPulseDecayFactor();
			// decay is 0..1 with 0 being rapid and 0.9999 being very slow
			// soo.... subtract from 1 and multiply by 256
			int decay = 255*(1.0f-decay_float);
			sendPulse( lights[j].getBoardId(), lights[j].getLightId(), brightness, decay );
			lights[j].resetWantsPulse();
		}
	}
	
	// clear serial flags
	for ( int j=0; j<lights.size(); j++ )
		lights[j].resetNeedsSerial();
	
	serial->endWrite();
}



void Lights::sendLightLevel( unsigned char board_id, unsigned char light_id, int brightness )
{
	// clamp
	if ( brightness < 0 )
		brightness = 0;
	if ( brightness > 4095 )
		brightness = 4095;
	// construct message
	unsigned char msg[3];
	msg[0] = (board_id) | 0x02; // set level
	msg[1] = (light_id<<4) + (brightness>>8);
	msg[2] = brightness & 0xff;
	//printf("Lights::sendLightLevel: writing msg %02x %02x %02x\n", msg[0], msg[1], msg[2] );
	// send msg
	serial->writeBytes( msg, 3 );
}

void Lights::sendPulse( unsigned char board_id, unsigned char light_id, int brightness, unsigned char decay )
{
	// clamp
	if ( brightness < 0 )
		brightness = 0;
	if ( brightness > 4095 )
		brightness = 4095;
	decay = min(255,max(0,(int)decay));
	
	// construct message
	unsigned char msg[4];
	msg[0] = (board_id) | 0x03; // pulse
	msg[1] = (light_id<<4) + (brightness>>8);
	msg[2] = brightness & 0xff;
	msg[3] = decay;
	//printf("Lights::sendPulse: writing msg %02x %02x %02x %02x\n", msg[0], msg[1], msg[2], msg[3] );
	// send msg
	serial->writeBytes( msg, 4 );
}


void Lights::pulse( int id, float max_bright, float decay_factor )
{
	if ( id >=0 && id < lights.size() ) 
	{ 
		lights[id].pulse( max_bright, decay_factor );
	}
}

void Lights::set( int id, float bright )
{
	lights[id].set( bright );
}

void Lights::pulseAll( float max_bright, float decay_factor )
{
	for ( int i=0; i<lights.size(); i++ ) 
	{
		pulse( i, max_bright, decay_factor );
	}
}

void Lights::clear( bool pummel )
{
	for ( int i=0; i<lights.size(); i++ ) 
	{
		lights[i].set( 0 );
	}
	// immediate
	flush();
	
	if ( pummel )
	{
		// write ALL BLACK ALL BLACK ALL BLACK lots of times
		serial->beginWrite();
		for ( int i=0; i<31; i++ )
		{
			unsigned char msg[2];
			msg[0] = 0x01;
			msg[1] = 0x00;
			serial->writeBytes( msg, 2 );
		}
		serial->endWrite();
	}
	else
	{
		// to be sure
		unsigned char msg[2];
		msg[0] = 0x01;
		msg[1] = 0x00;
		serial->writeBytes( msg, 2 );
	}
}

void Lights::draw()
{
	for ( int i=0; i<lights.size(); i++ )
	{
		lights[i].draw();
	}
}

void Lights::illuminateCircularArea( float x, float y, float area )
{
	for ( int i=0; i<lights.size(); i++ )
	{
		
		float dx = lights[i].getX()-x;
		float dy = lights[i].getY()-y;
		if ( dx*dx+dy*dy <= area*area )
		{
			float brightness = 1.0f-(dx*dx+dy*dy)/(area*area);
			pulse( i, brightness, 0 );
		}
	}
}

void Lights::sendEveryLightLevel( unsigned char board_id, unsigned char* data )
{
	unsigned char func = FUNC_SET_EVERY | board_id;
	serial->writeBytes( &func, 1 );
	serial->writeBytes( data, 24 );
	
	printf(" %02x\n", func );
	for ( int i=0; i<8; i++ )
	{
		unsigned int l1 =  (((unsigned int)(data[i*3]))<<4) + ((data[i*3+1]&0xf0)>>4);
		unsigned int l2 = (unsigned int)((data[i*3+1]&0x0f)<<8) + data[i*3+2];
		printf(" %02x %02x %02x -> %03x %03x;%s", data[i*3], data[i*3+1], data[i*3+2], l1, l2, i%4==3?"\n":"" );
	}
	// done
}

void Lights::illuminateCorridor( float x, float y, float dx, float dy, float power, float width )
{
	ofxVec3f p(x,y);
	ofxVec3f d(dx,dy);
	d.normalize();
	// loop through all the lights
	//printf("line segment is (%f,%f)-(%f,%f), width %f\n", x, y, dx+x, dy+y, width );
	for ( int i=0; i<lights.size(); i++ )
	{
		Light& light = lights[i];
		ofxVec3f light_pos( light.getX(), light.getY() );
		float line_position;
		// get the nearest point on the given infinite line
		ofxVec3f nearest_point_on_line = NearestPointOnLineToPoint::calculateNearestPoint
			( p, p+d, light_pos, &line_position );
		// work out the distance
		float distance = (nearest_point_on_line-light_pos).length();
		//printf("  nearest point is %f %f (%f) -> distance %f\n", nearest_point_on_line.x, nearest_point_on_line.y, 
		//	   line_position, distance );
		// illuminate if we should
		if ( distance <= width )
		{
			float brightness;
			if ( distance > 0 )
				brightness = (1.0f-distance/width);
			else
				brightness = 1;
			set( i, power*brightness*brightness );
		}			
		else
			set( i, 0 );
	}
}
void Lights::drawIlluminateCorridor( float x, float y, float dx, float dy, float power, float width )
{
	ofxVec3f p(x,y);
	ofxVec3f d(dx,dy);
	d.normalize();
	// loop through all the lights
	//printf("line segment is (%f,%f)-(%f,%f), width %f\n", x, y, dx+x, dy+y, width );
	ofSetColor( 0x20, 0x20, 0xa0 );
	ofLine( x*ofGetWidth(), y*ofGetHeight(), (x+dx)*ofGetWidth(), (y+dy)*ofGetHeight() );
	ofSetColor( 0x20, 0x20, 0x20 );
	for ( int i=0; i<lights.size(); i++ )
	{
		Light& light = lights[i];
		ofxVec3f light_pos( light.getX(), light.getY() );
		float line_position;
		// get the nearest point on the given infinite line
		ofxVec3f nearest_point_on_line = NearestPointOnLineToPoint::calculateNearestPoint
			( p, p+d, light_pos, &line_position );
		// work out the distance
		ofLine( light.getX()*ofGetWidth(), light.getY()*ofGetHeight(), 
			   nearest_point_on_line.x*ofGetWidth(), nearest_point_on_line.y*ofGetHeight() );
	}
}

void Lights::latch( unsigned char board_id )
{
	unsigned char func = FUNC_LATCH | board_id;
	serial->writeBytes( &func, 1 );
	// done
}

LightsDelaunay* Lights::getDelaunay()
{
	if ( rebuild_delaunay )
	{
		if ( delaunay != NULL )
		{
			delete delaunay;
		}
		delaunay = new LightsDelaunay( *this );
	}
	return delaunay;
}

void Lights::toggleLightIsBig( int id ) {
	lights.at(id).toggleBig();
	if ( lights.at(id).isBig() )
	{
		big_lights.push_back( id );
		std::sort( big_lights.begin(), big_lights.end() );
	}
	else
	{
		vector<int>::iterator found = std::find( big_lights.begin(), big_lights.end(), id );
		if ( found != big_lights.end() )
			big_lights.erase( found );
	}
}


