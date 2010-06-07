/*
 *  Animation.h
 *  serialExample
 *
 *  Created by damian on 07/05/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "Lights.h"

class Animation
{
public:
	Animation( Lights* _lights ) { lights = _lights; }
	virtual ~Animation() {};
	
	virtual void update( float ms ) {};
	virtual void draw() {};
	
	virtual void keyPressed( int key ) {};
	virtual void mouseMoved( int x, int y ) {};

protected:

	Lights* lights;


};


class AnimationFactory
{
public:
	static void useLights( Lights* _lights ) { lights = _lights; }
	static Animation* makeAnimation( string name );
private:
	static map< string, Animation* > cache;
	static Lights* lights;
};


class AnimationSwitcher
{
public:
	AnimationSwitcher() { current = 0; }
	void addAnim( string name ) { anims.push_back( name ); }

	Animation* goToAnim( string name ) { 
		for ( int i=0; i<anims.size(); i++ ) 
		{ 
			if ( anims[i] == name ) 
			{ 
				printf("going to anim '%s'\n", name.c_str() ); 
				current = i; 
			} 
			return currentAnim(); 
		}
	};
	
	Animation* currentAnim() { return AnimationFactory::makeAnimation( anims[current] ); }
	Animation* nextAnim() { current++; if ( current >= anims.size() ) current = 0 ; printf("going to anim '%s'\n", anims[current].c_str() ); return AnimationFactory::makeAnimation( anims[current] ); }
	Animation* prevAnim() { current--; if ( current < 0 ) current = anims.size()-1; printf("going to anim '%s'\n", anims[current].c_str() ); return AnimationFactory::makeAnimation( anims[current] ); }
	
	void draw();
	
private:
	vector<string> anims;
	int current;
};
