#include "testApp.h"

#include "dummy_of.h"

void runDummyOf( testApp* ptr )
{
	ptr->setup();
	while ( true )
	{
		ptr->update();
		// limit to max 100fps
		usleep( 10000 );
	}
}

/*
 * void ofSetColor( int,int,int,int )
{

}

int ofGetWidth()
{
	return 1024;
}
int ofGetHeight()
{
	return 768;
}

void ofLine( float,float,float,float )
{
}

void ofCircle( int,int,int )
{
}
*/
