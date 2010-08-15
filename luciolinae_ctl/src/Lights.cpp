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



static const int board_ids[16] = { 0x10, 0x20, 0x30, 0x40, 0x50, 0x60, 0x70, 0x80, 0x90, 0xa0, 0xb0, 0xc0, 0xd0, 0xe0, 0xf0 };


Lights::~Lights()
{ 
	if ( delaunay ) delete delaunay; 
	for ( map< int, LightsDelaunay* >::iterator it = boards_delaunay.begin();
		 it != boards_delaunay.end();
		 ++it )
	{
		delete (*it).second;
	}
}



void Lights::setup( BufferedSerial* _serial )
{
	// ** REMEMBER TO DO THE OTHER SETUP
	
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
	
	debug_sent.resize( lights.size() );

	clear( true );
}

void Lights::setup( BufferedSerial* _serial, ofxXmlSettings& data )
{
	// ** REMEMBER TO EDIT THE OTHER SETUP
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
//	groups.resize( 10 );
/*	int num_groups = data.getNumTags( "group" );
	for ( int i=0; i<10; i++ ) 
	{
		data.pushTag
	}*/
	
	printf("loaded %i lights\n", num_lights );
	std::sort( big_lights.begin(), big_lights.end() );
	data.popTag();
	
	debug_sent.resize( lights.size() );
	
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
	// send serial
	flush();

	// update lights
	for ( int i=0;i<lights.size(); i++ )
		lights[i].update(elapsed);
	
}

void Lights::flush()
{
	//printf("flush called\n");
	serial->beginWrite();
	
	// limit total current;
	float total_light = 0;
	for ( int i=0; i<lights.size(); i++ )
	{
		float bright = lights[i].getBrightness();
		total_light += bright*bright;
	}
	
	// 15A max current
	float bright_factor = 1;
	if ( total_light > 15.0 )
	{
		bright_factor = sqrtf( 15.0f/total_light );
	}
	
	vector<int> counts;
	// fill with 0
	counts.insert( counts.begin(), num_boards, 0 );
	// count up how many per board
	for ( int i=0; i<lights.size(); i++ )
	{
		if ( lights[i].needsSerial() /*&& !lights[i].wantsPulse()*/ )
			counts[(lights[i].getBoardId()>>4)-1]++;
	}
	for ( int i=0; i<num_boards; i++ )
	{
		// if more than 8, best to send all at once
		if ( counts[i] > 8 )
		{
		//	printf("compiling every light level data for board %i\n", i);
			unsigned char total_board_data[24];
			for ( int j=0; j<lights.size(); j++ )
			{
				// find this board's lights
				if ( lights[j].getBoardId()==(i+1)<<4 )
				{
					// add data to array
					int light_id = lights[j].getLightId();
					int base_index = /*((15-light_id)*3>>1);*/ (light_id*3)>>1;
					float bright = lights[j].getBrightness();
					if ( !lights[j].isBig() )
						bright *= small_bright_factor;
					bright *= bright_factor;
					bright *= bright;
					unsigned int brightness = min(4095,max(0,int(bright*4096)));
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
					// clear serial
					lights[j].resetNeedsSerial();
				}
			}
			//printf("sending every light level data\n");
			// now send total_board_data
			sendEveryLightLevel( (i+1)<<4, total_board_data );
		}
		else
		{
			//printf("sending levels for board %i individually\n", i );
			// send lights for this board individually
			for ( int j=0; j<lights.size(); j++ )
			{
				// find this board's lights
				if ( (lights[j].getBoardId()==(i+1)<<4) && lights[j].needsSerial() /*&& !lights[j].wantsPulse()*/ )
				{
					float bright = lights[j].getBrightness();
					if ( !lights[j].isBig() )
						bright *= small_bright_factor;
					bright *= bright_factor;
					bright *= bright;
					int brightness = bright*4096;
					sendLightLevel( lights[j].getBoardId(), lights[j].getLightId(), brightness );
					lights[j].resetNeedsSerial();
				}
			}
		}
	}
	
	// latch in the data
	latch();

	// send pulses
	/*
	for ( int j=0; j<lights.size(); j++ )
	{
		// find this board's lights
		if ( lights[j].needsSerial() && lights[j].wantsPulse() )
		{
			float bright = 	lights[j].getBrightness();
			if ( !lights[j].isBig() )
				bright *= small_bright_factor;
			bright *= bright;
			int brightness = bright*4096;
			float decay_float = lights[j].getPulseDecayFactor();
			// decay is 0..1 with 0 being rapid and 0.9999 being very slow
			// soo.... subtract from 1 and multiply by 256
			int decay = 255*(1.0f-decay_float);
			sendPulse( lights[j].getBoardId(), lights[j].getLightId(), brightness, decay );
			lights[j].resetWantsPulse();
		}
	}*/
	
	
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
	unsigned char msg[4];
	msg[0] = (board_id) | 0x02; // set level
	msg[1] = (light_id<<4) + (brightness>>8);
	msg[2] = brightness & 0xff;
	msg[3] = calculateCRC( msg, 3 );
	//printf("Lights::sendLightLevel: writing msg %02x %02x %02x\n", msg[0], msg[1], msg[2] );
	// send msg
	serial->writeBytes( msg, 4 );
	
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
	unsigned char msg[5];
	msg[0] = (board_id) | 0x03; // pulse
	msg[1] = (light_id<<4) + (brightness>>8);
	msg[2] = brightness & 0xff;
	msg[3] = decay;
	msg[4] = calculateCRC( msg, 4 );
	//printf("Lights::sendPulse: writing msg %02x %02x %02x %02x\n", msg[0], msg[1], msg[2], msg[3] );
	// send msg
	serial->writeBytes( msg, 5 );
}


void Lights::pulse( int id, float bright, bool include_big, float end_bright )
{
	if ( id >=0 && id < lights.size() ) 
	{ 
		if ( !lights[id].isBig() || include_big )
		{
			if ( blend_state_stack.back().blending )
			{
				float current = lights[id].getBrightness();
				if ( blend_state_stack.back().blend_mode == BLEND_MIX )
					bright = current*(1-blend_state_stack.back().blend_alpha) + bright*blend_state_stack.back().blend_alpha;
				else if ( blend_state_stack.back().blend_mode == BLEND_ADD )
					bright = min( 1.0f, current + bright*blend_state_stack.back().blend_alpha );
			}
			lights[id].pulse( bright, end_bright );
		}
	}
}

void Lights::set( int id, float bright, bool include_big )
{
	if ( lights[id].isBig() && !include_big )
		return;
	
	if ( blend_state_stack.back().blending ) 
	{
		float current = lights[id].getBrightness() ;
		if ( blend_state_stack.back().blend_mode == BLEND_MIX )
		{
			// current*(1-blend_alpha) + bright*blend_alpha
			bright = current*(1-blend_state_stack.back().blend_alpha) + bright*blend_state_stack.back().blend_alpha;
		}
		else if ( blend_state_stack.back().blend_mode == BLEND_ADD )
		{
			// current + bright*blend_alpha
			// keep to bounds
			bright = min( 1.0f, current + bright*blend_state_stack.back().blend_alpha );
		}
	}
	lights[id].set( bright );
}

void Lights::pulseAll( float max_bright, bool include_big )
{
	for ( int i=0; i<lights.size(); i++ ) 
	{
		pulse( i, max_bright, include_big, 0  );
	}
}

void Lights::clear( bool pummel )
{
	for ( int i=0; i<lights.size(); i++ ) 
	{
		lights[i].pulse( 0, 0 );
		lights[i].set( 0 );
	}
	// immediate
	flush();
	
	if ( pummel )
	{
		// write ALL BLACK ALL BLACK ALL BLACK lots of times
		serial->beginWrite();
		for ( int j=0; j<16; j++ )
		{
			unsigned char black[24];
			memset( black, 0, 24 );
			sendEveryLightLevel( (j+1)<<4, black );			
			for ( int i=0; i<4; i++ )
			{
				unsigned char msg[3];
				msg[0] = ((unsigned char)((j+1)<<4))|0x01;
				msg[1] = 0x00;
				msg[2] = calculateCRC( msg, 2 );
				serial->writeBytes( msg, 3 );
				
			}
			latch();
		}
		serial->endWrite();
	}
	else
	{
		serial->beginWrite();
		// to be sure
		unsigned char msg[3];
		msg[0] = 0x01;
		msg[1] = 0x00;
		msg[2] = calculateCRC( msg, 2 );
		serial->writeBytes( msg, 3 );
		latch();
		serial->endWrite();
	}
}

void Lights::draw()
{
	for ( int i=0; i<lights.size(); i++ )
	{
		lights[i].draw();
	}
}

void Lights::illuminateCircularArea( float x, float y, float area, bool include_big )
{
	for ( int i=0; i<lights.size(); i++ )
	{
		
		float dx = lights[i].getX()-x;
		float dy = lights[i].getY()-y;
		if ( dx*dx+dy*dy <= area*area )
		{
			float brightness = 1.0f-(dx*dx+dy*dy)/(area*area);
			pulse( i, brightness, 0, include_big );
		}
	}
}

void Lights::sendEveryLightLevel( unsigned char board_id, unsigned char* data )
{
	unsigned char func = FUNC_SET_EVERY | board_id;
	// calculate crc

	serial->writeBytes( &func, 1 );
	serial->writeBytes( data, 24 );
	// write crc
	unsigned char crc = calculateCRC( &func, 1 );
	crc = updateCRC( data, 24, crc );
	serial->writeBytes( &crc, 1 );
	
	/*
	printf(" %02x\n", func );
	for ( int i=0; i<8; i++ )
	{
		unsigned int l1 =  (((unsigned int)(data[i*3]))<<4) + ((data[i*3+1]&0xf0)>>4);
		unsigned int l2 = (unsigned int)((data[i*3+1]&0x0f)<<8) + data[i*3+2];
		printf(" %02x %02x %02x -> %03x %03x;%s", data[i*3], data[i*3+1], data[i*3+2], l1, l2, i%4==3?"\n":"" );
	}
	*/
	// done
}

void Lights::illuminateCorridor( float x, float y, float dx, float dy, float power, float width, bool include_big )
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
		ofxVec3f nearest_point_on_line = NearestPointOnLineToPoint::calculateNearestPoint( 
			p, p+d, light_pos, &line_position );
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
			set( i, power*brightness*brightness, include_big );
		}			
		else
			set( i, 0, include_big );
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
	unsigned char crc = calculateCRC( &func, 1 );
	serial->writeBytes( &crc, 1 );
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
		rebuild_delaunay = false;
	}
	return delaunay;
}

LightsDelaunay* Lights::getBoardDelaunay( int board_id )
{
	if ( rebuild_boards_delaunay )
	{
		for ( int i=0; i<num_boards; i++ )
		{
			if ( boards_delaunay[board_ids[i]] != NULL );
			{
				delete boards_delaunay[board_ids[i]];
			}
			boards_delaunay[board_ids[i]] = new LightsDelaunay( *this, board_ids[i] );
		}
		rebuild_boards_delaunay = false;
	}
	return boards_delaunay[board_id];
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


int Lights::getLightIndexForBig( int search ) const
{
	// locate i in big lights
	for ( int i=0; i<big_lights.size(); i++ )
	{
		if ( big_lights[i] == search )
			return i;
	}
	// not found
	return -1;
}


void Lights::illuminateRect( float x, float y, float w, float h, float power, bool include_big )
{
	for ( int i=0; i<lights.size(); i++ )
	{
		const Light& light = lights[i];
		float lx = light.getX();
		float ly = light.getY();
		if ( lx > x && lx < x+w &&
			ly > y && ly < y+h )
		{
			if ( !light.isBig() || include_big )
			{
				pulse( i, power, 0, include_big );
			}
		}
	}
}


unsigned char Lights::updateCRC( unsigned char* data, int length, unsigned char seed = 0 )
{
	for ( int i=0; i<length; i++ )
	{
		unsigned char input = data[i];
		
		unsigned char feedback;
		for (unsigned char j=0; j<8; j++)
		{
			feedback = ((seed ^ input) & 0x01);
			if (!feedback) seed >>= 1;
			else
			{
				seed ^= 0x18;
				seed >>= 1;
				seed |= 0x80;
			}
			input >>= 1;
		}
	}
	
	return seed;
}


vector<int> Lights::getLightIdsForBoard( int board_id ) const
{
	vector<int> results;
	for ( int i=0; i<lights.size(); i++ )
	{
		if ( lights[i].getBoardId() == board_id )
			results.push_back(i);
	}
	return results;
}
