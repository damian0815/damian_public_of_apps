/*
 *  IKBone.h
 *  emptyExample
 *
 *  Created by damian on 18/06/10.
 *  Copyright 2010 frey damian@frey.co.nz. All rights reserved.
 *
 */

class IKBone
{
public:
	void IKBone( float _length, float _angle, float _weight_centre )
	: length(_length), angle(_angle), weight_centre(_weight_centre)
	{};

private:

	float length, angle, weight_centre;

};
