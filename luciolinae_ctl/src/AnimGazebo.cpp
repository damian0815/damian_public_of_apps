/*
 *  AnimGazebo.cpp
 *  luciolinae_ctl
 *
 *  Created by damian on 18/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#include "AnimGazebo.h"
#include "LightsDelaunay.h"
#include "ofxVec2f.h"
#include "Util.h"

#include "NearestPointOnLineToPoint.h"

static const float STEP_TIME = 0.08f;

const char* AnimGazebo::NAME = "Gazebo";

AnimGazebo::AnimGazebo( Lights* _lights )
: Animation( _lights ) 
{ 
	inout = false; 
	which = ofRandom( 0, lights->getNumLights()*0.999f );
	closest_a = -1;
	editing_delaunay = false;
	draw_delaunay = true;
	speed = 1.0f;
	
	customDelaunay = *lights->getDelaunay();
	
	ofxXmlSettings gazebo_settings;
	gazebo_settings.loadFile("gazebo_settings.xml");
	gazebo_settings.pushTag( "gazebo" );
	gazebo_settings.pushTag( "delaunay" );
	int num_delaunay = gazebo_settings.getNumTags( "removed" );
	for ( int i=0; i<num_delaunay; i++ )
	{
		gazebo_settings.pushTag("removed", i );
		int a = gazebo_settings.getValue( "a", -1 );
		int b = gazebo_settings.getValue( "b", -1 );
		customDelaunay.removeNeighbour( a, b );
		removed_neighbours.push_back( make_pair(a, b) );

		gazebo_settings.popTag();
	}
	gazebo_settings.popTag();
	gazebo_settings.popTag();
	
	timer = 5.0f;
	
};


void AnimGazebo::saveDelaunay()
{
	ofxXmlSettings gazebo_settings;
	gazebo_settings.addTag( "gazebo" );
	gazebo_settings.pushTag( "gazebo" );
	gazebo_settings.addTag( "delaunay" );
	gazebo_settings.pushTag( "delaunay" );
	for ( int i=0; i<removed_neighbours.size(); i++ )
	{		
		gazebo_settings.addTag("removed");
		gazebo_settings.pushTag("removed", i );
		gazebo_settings.addValue( "a", removed_neighbours[i].first );
		gazebo_settings.addValue( "b", removed_neighbours[i].second );
		gazebo_settings.popTag();
	}
	gazebo_settings.popTag();
	gazebo_settings.popTag();
	
	gazebo_settings.saveFile("gazebo_settings.xml");
}



void AnimGazebo::update( float elapsed )
{
	timer -= elapsed;
	if ( timer < 0.0f )
	{
		float rand = squaredRandom(0.0f, 1.0f);
		
		speed = 0.2f+rand*1.6f+ofRandom( 0.0f, 0.2f );
		which = ofRandom( 0.0f, 0.9999f*lights->getNumLights() );
		triggerDelaunay( which );
		//timer = 2.0f+(1.0f-rand)*8.0f+ofRandom(0.0f, 8.0f);
		timer = squaredRandom( 2.0f, 15.0f );
	} 
}

void AnimGazebo::keyPressed ( int k ) 
{
	if ( editing_delaunay )
	{
		//printf("key %i (del %i bs %i)\n", k, OF_KEY_DEL, OF_KEY_BACKSPACE );
		if ( k == OF_KEY_BACKSPACE )
			deleteCurrentEdge();
		else if ( k == 'u' )
			undoDeleteEdge();
		else if ( k == 'W' )
		{
			saveDelaunay();
			printf("AnimGazebo delaunay saved\n");
		}
		else if ( k == 'E' )
		{
			printf("leaving AnimGazebo delaunay edit\n");
			editing_delaunay = false;
		}
	}
	else
	{
		if ( k == 'w' )
		{
			which = ofRandom( 0.0f, 0.9999f*lights->getNumLights() );
		}
		else if ( k == 'i' )
		{
			inout = !inout;
		}
		else if ( k == 'E' )
		{
			printf("entering AnimGazebo delaunay edit\n");
			editing_delaunay = true;
			closest_a = -1;
		}
		else if ( k == 's' )
			speed *= 1.1f;
		else if ( k == 'S' )
			speed /= 1.1f;
		else if ( k == 'd' )
			draw_delaunay = !draw_delaunay;
	}

	if ( k == OF_KEY_RETURN )
	{
		triggerDelaunay( which );
	}
}


void AnimGazebo::undoDeleteEdge()
{
	if ( !removed_neighbours.empty() )
	{
		customDelaunay.addNeighbour( removed_neighbours.back().first, removed_neighbours.back().second );
		removed_neighbours.erase( removed_neighbours.end()-1 );
	}
}

void AnimGazebo::deleteCurrentEdge()
{
	if ( closest_a != -1 )
	{
		customDelaunay.removeNeighbour( closest_a, closest_b );
		removed_neighbours.push_back( make_pair(closest_a, closest_b) );
	}
}


void AnimGazebo::triggerDelaunay( int index )
{
	index = min(lights->getNumLights()-1,max(0,index));
	if ( inout )
		DelaunayPulses::getInstance()->addPulseIn( index, 0.7f, 1.0f, speed, &customDelaunay );
	else
		DelaunayPulses::getInstance()->addPulseOut( index, 1.0f, 0.3f, speed, &customDelaunay );
}


void AnimGazebo::draw()
{
	if ( editing_delaunay || draw_delaunay )
	{
		for ( int i=0; i<lights->getNumLights(); i++ )
		{
			set<int> adj = customDelaunay.getNeighbours( i );
			for ( set<int>::iterator jt = adj.begin();
				 jt != adj.end();
				 ++jt )
			{
				if ( closest_a == i && closest_b == *jt )
					ofSetColor( 0xF0, 0x10, 0x10, 0x80 );
				else
					ofSetColor( 0x20, 0x80, 0x20, 0x18 );
				ofLine( ofGetWidth()*lights->getLight( i ).getX(),   ofGetHeight()*lights->getLight( i ).getY(),
					   ofGetWidth()*lights->getLight( *jt ).getX(), ofGetHeight()*lights->getLight( *jt ).getY() );
			}
		}
	}	
	
	char buf[1024];
	sprintf(buf, "speed: %5.2f", speed );
	ofSetColor( 0xff, 0xff, 0xff, 0x80 );
	ofDrawBitmapString( buf, 10, ofGetHeight()-15 );
}


void AnimGazebo::mouseMoved( int x, int y)
{
	
	static const float DISTANCE_THRESH = 10.0f;
	ofxVec3f mouse( x, y, 0 );
	
	float closest_distance;
	closest_a = -1;
	
	for ( int i=0; i<lights->getNumLights(); i++ )
	{
		set<int> adj = customDelaunay.getNeighbours(i);
		int count = 0;
		for ( set<int>::iterator jt = adj.begin();
			 jt != adj.end();
			 ++jt, ++count )
		{
			// calculate distance to line
			const Light& l_a = lights->getLight( i );
			const Light& l_b = lights->getLight( *jt );
			ofxVec3f a( ofGetWidth()*l_a.getX(), ofGetHeight()*l_a.getY(), l_a.getZ() );
			ofxVec3f b( ofGetWidth()*l_b.getX(), ofGetHeight()*l_b.getY(), l_b.getZ() );
			float line_pct;
			ofxVec3f nearest = NearestPointOnLineToPoint::calculateNearestPoint( a, b, mouse, &line_pct, true );
				
			// distance is ..
			float distance = (nearest-mouse).length();
			if ( distance < DISTANCE_THRESH )
			{
				if ( closest_a == -1 || distance < closest_distance )
				{
					closest_distance = distance;
					closest_a = i;
					closest_b = (*jt);
				}
			}
		}
	}
	
}

