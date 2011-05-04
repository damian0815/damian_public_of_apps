#include "testApp.h"

#include "ofxDirList.h"

static const float TEXT_FADE_SPEED = 1.0f;

void testApp::getFiles()
{
	ofxDirList lister;
	files.clear();
	
	int count = lister.listDir( "" );
	for ( int i=0; i<count; i++ ) 
	{
		files.push_back( lister.getName( i ) );
	}
	sort( files.begin(), files.end() );

	// don't overflow
	currFile = max(0,min(currFile,int(files.size())));
}

//--------------------------------------------------------------
void testApp::setup(){

	
	video = NULL;
	ofSetLogLevel( OF_LOG_NOTICE );
	ofSetFrameRate( 24 );
	ofSetVerticalSync(true);
	
	font.loadFont( "fonts/Menlo.ttc", 32 );
	
	text_alpha = 0.0f;
	getFiles();
	currFile = -1;
	setCurrFile(0);
}

//--------------------------------------------------------------
void testApp::update(){

	if ( video )
	{
		video->update();
	}
	if ( !video || videoStarted )
	{
		text_alpha = max(0.0,text_alpha-TEXT_FADE_SPEED*ofGetLastFrameTime());
	}
}


void testApp::drawInRectAspectFit( ofBaseDraws* drawable, ofRectangle rect )
{
	float image_aspect = drawable->getWidth()/drawable->getHeight();
	float frame_aspect = rect.width/rect.height;

	ofRectangle image_draw_frame = rect;
	if ( image_aspect>frame_aspect )
	{
		// image is wider + shorter than frame
		// shrink the frame vertically
		float pct = frame_aspect/image_aspect;
		image_draw_frame.height *= pct;
		float diff = rect.height - image_draw_frame.height;
		image_draw_frame.y += diff/2.0f;
	}
	else
	{
		// image is narrower + taller than frame
		// shrink the frame horizontally
		float pct = image_aspect/frame_aspect;
		image_draw_frame.width *= pct;
		float diff = rect.width - image_draw_frame.width;
		image_draw_frame.x += diff/2.0f;
	}
	drawable->draw( image_draw_frame.x, image_draw_frame.y, image_draw_frame.width, image_draw_frame.height );
	
}


//--------------------------------------------------------------
void testApp::draw(){

	ofBackground( 0 );

	ofSetColor( 255,255,255 );
	ofRectangle screen( 0, 0, ofGetWidth(), ofGetHeight() );
	if ( image.width > 0 )
	{
		drawInRectAspectFit( &image, screen );
	}
	else if ( video && videoStarted )
	{
		drawInRectAspectFit( video, screen );
	}
	
	if ( files.size()>0 )
	{
		string progress;
		if ( video )
		{
			int crossover = video->getPosition()*47;
			char buf[49];
			for ( int i=0; i<48; i++ )
			{
				if ( i==crossover )
					buf[i] = '|';
				else
					buf[i] = ' ';
			}
			buf[48] = 0;
			progress = "["+string(buf)+"]";
		}
		ofSetColor( 255,255,255,text_alpha*255 );
		font.drawString(  files[currFile], 10, ofGetHeight()-13 );
		font.drawString(  progress.c_str(), 5, ofGetHeight()-60 );
		
//		ofDrawBitmapString( files[currFile]+suffix, 10, 13 );
	}
	
}

void testApp::setCurrFile( int index )
{
	if ( files.size() == 0 )
		return;

	index = min(int(files.size())-1,index);
	if ( index != currFile && index != -1 )
	{
		if ( image.width>0 )
			image.clear();
		if ( video )
		{
			video->stop();
			video->closeMovie();
			delete video;
			video = NULL;
		}
		soundPlayer.stop();
		currFile = index;
	
		ofLog(OF_LOG_NOTICE, "trying '%s'..", files[currFile].c_str() );
		bool imageLoaded = image.loadImage(files[currFile]);
		if ( imageLoaded )
		{
			ofLog(OF_LOG_NOTICE, "loaded as image %ix%i", image.width, image.height );
			// try to load audio as well
			string expected_audio = files[currFile];
			expected_audio.replace( expected_audio.size()-4, 3, "WAV" );
			FILE* test = fopen (ofToDataPath( expected_audio ).c_str(), "r" );
			if ( test ) 
			{				
				fclose( test );
				soundPlayer.loadSound( expected_audio, false );
				soundPlayer.play();
			}
		}
		if ( !imageLoaded || image.width==0 )
		{
			// must be a movie
			video = new ofVideoPlayer();
			bool movieLoaded = video->loadMovie(files[currFile]);
			if ( movieLoaded )
			{
				ofLog(OF_LOG_NOTICE, "loaded as video %ix%i", video->width, video->height );
				video->play();
				video->setLoopState(OF_LOOP_NONE);
				video->setPaused(true);
				videoStarted = false;
			}
			else
			{
				delete video;
				video = NULL;
				ofLog(OF_LOG_ERROR, "not loaded" );
			}
		}
	}	
	text_alpha = 1.0f;
	
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

	if ( key == OF_KEY_RIGHT )
		setCurrFile( currFile+1 );
	//currFile = max(0,min(int(files.size())-1,currFile+1));
	else if ( key == OF_KEY_LEFT )
		setCurrFile( currFile-1 );
	//currFile = max(0,currFile-1);
	else if ( key == 'f' )
		ofToggleFullscreen();
	
	// video controls
	if ( video )
	{
		if ( key == ' ' )
		{
			videoStarted = true;
			video->setPaused( !video->isPaused() );
		}
		else if ( key == '0' )
			video->setPosition(0);
		else if ( key == '[' )
			video->setPosition( video->getPosition()-(5.0f/video->getDuration() ) );
		else if ( key == ']' )
			video->setPosition( video->getPosition()+(5.0f/video->getDuration() ) );

		if ( key != ' ' )
			text_alpha = 1.0f;
	}	
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

