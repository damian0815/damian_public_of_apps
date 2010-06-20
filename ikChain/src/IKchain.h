/*
 *  IKchain.h
 *  emptyExample
 *
 *  Created by damian on 18/06/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

#pragma once

class IKChain
{
public:
	
	// weight_centre determines which end of the bone moves:
	// 0   = the start (==parent link) only 
	// 1   = the end   (==child link) only
	// 0.5 = start and end both move the same amount
	void addBone( float length, float angle, float weight_centre=0.5f );
	



private:
	vector<IKBone> bones;

	ofxVec2f root_pos;


};


