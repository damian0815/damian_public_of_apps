/*
 * a single 2D point in floating point coordinates
 * packed with 2 integers for use in array/hashset
 * 
 * this file also define the rounding of coordinates: they are snapped to a grid
 * with steps of '2^(-rounding_power)'  ( currently 1/64 )
 */

#ifndef FPOINT_H_
#define FPOINT_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "MComparator.h"
#include "MHashor.h"
#include "MPoint.h"

// constant for rounding
// the points in the rounding grid are '2^(-rounding_power)' apart, to make use
// of the ldexpf() function, thus avoiding divisions/multiplications (hopefully)
#define rounding_power 6

class FPoint : public MPoint<float>
{
public:
	int		_no;
	int		_tmp;
	
	FPoint(void);
	FPoint(const FPoint &o);
	FPoint(const MPoint<float> &o);
	FPoint(const MPoint<double> &o);
	FPoint(float x, float y);
	virtual ~FPoint(void);
	
	FPoint&		operator=(FPoint const &o) {
		_x = o._x;
		_y = o._y;
		_no = o._no;
		return *this;
	};
	FPoint&		operator=(MPoint<float> const &o) {
		_x = o._x;
		_y = o._y;
		return *this;
	};
	FPoint&		operator=(MPoint<double> const &o) {
		_x = o._x;
		_y = o._y;
		return *this;
	};
	// for use in hashsets
	int			getNext(void);
	void		setNext(int to);
	// misc.
	bool		equals(const FPoint &o) const ;
	void		dump(void);
	// round this point
	void		round(void);
};

// utility class for sorting an array of FPoints by position
class FPointCmpByPos : public MComparator<FPoint> {
public:
	FPointCmpByPos(void) {};
	virtual ~FPointCmpByPos(void) {};
	virtual bool	equals(FPoint const &a, FPoint const &b) const {
		return a._x == b._x && a._y == b._y;
	};
	virtual int	compare(FPoint const &a, FPoint const &b) const {
		if ( a._y < b._y ) return -1;
		if ( a._y > b._y ) return 1;
		if ( a._x < b._x ) return -1;
		if ( a._x > b._x ) return 1;
		return 0;
	};
};

// utility class for using FPoints in a hashset
// only works with ROUNDED FPoints (otherwise the equality test won't work properly)
class FPointHashByPos : public MHashor<FPoint> {
public:
	FPointHashByPos(void) {};
	virtual ~FPointHashByPos(void) {};
	
	virtual	bool	equals(FPoint const &a, FPoint const &b) const {
		// since we limit ourselves to rounded points, simple float equality tests
		// are enough
		return a._x == b._x && a._y == b._y;
	};
	virtual	int		hash(FPoint const &a) const {
		int h;
		// get the coordinates on the grid, and use that for hashing
		// using floats directly would call for another hash function
		int ix = (int) floorf(ldexpf(a._x, rounding_power));
		int iy = (int) floorf(ldexpf(a._y, rounding_power));
		// very dumb hash
		h = (13251153 * ix) + iy;
		if ( h < 0 ) h = -h;
		return h;
	};
};

#endif /*FPOINT_H_*/
