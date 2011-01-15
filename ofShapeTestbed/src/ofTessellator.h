/*
 *  ofTessellator.h
 *  openFrameworks
 *
 *  Created by theo on 28/10/2009.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#pragma once

#include "ofConstants.h"
#include "ofMain.h"
#include "ofShape.h"
#include "ofVboMesh.h"

#ifndef CALLBACK
#define CALLBACK
#endif


/** ofTessellator
 
 Tessellates a polyline into a number of meshes, conforming to the given winding rule.
 
 @author Adapted by damian from Theo's ofxShape.
 */



class ofTessellator
{
public:	
	
	/// tessellate polyline and return a mesh. if bIs2D==true, do a 10% more efficient normal calculation.
#ifdef DRAW_WITH_MESHIES
	static vector<meshy> tessellate( const ofPolyline& polyline, int polyWindingMode, bool bFilled, bool bIs2D=false );
#else
	static ofVboMesh tessellate( const ofPolyline& polyline, bool bFilled, bool bIs2D=false );
#endif

	
private:
	
	
	/// clear out everything
	static void clear();

	/// callbacks for GLU tesselation
	static void CALLBACK error(GLenum);
	static void CALLBACK vertex( void* data);
	static void CALLBACK combine( GLdouble coords[3], void* vertex_data[4], GLfloat weight[4], void** outData);
	
	static void CALLBACK begin(GLint type);
	static void CALLBACK end();
		
	/// ensure thread-safety
	static ofMutex mutex;

	
	// filled during tessellation
	static ofVboMesh resultMesh;
	static GLint currentTriType; // GL_TRIANGLES, GL_TRIANGLE_FAN or GL_TRIANGLE_STRIP
	static vector<ofPoint> vertices;
	
	//---------------------------- for combine callback:
	static std::vector <double*> newVertices;
	//---------------------------- store all the polygon vertices:
	static std::vector <double*> ofShapePolyVertexs;
	

	
	static vector<meshy> resultMeshies;
	
};


