/*
 * a 2D point in integer coordinates, representing a pixel, with a float value, used to
 * store the delta in coverage with respect to the pixel on the left
 * 
 * this class is meant to be used in a hashset, and then sorted according to the position
 */

#ifndef IPOINT_H_
#define IPOINT_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "MComparator.h"
#include "MHashor.h"
#include "MPoint.h"

class IPoint : public MPoint<int>
{
public:
	int		_hash;	// position will not change, so the hash will not change either
	                // so it can be computed once, at instanciation time
	float	_val;   // value
	int		_tmp;   // for storing the 'next' index in the hashset
	
	IPoint(void);
	IPoint(const IPoint &o);
	IPoint(int x, int y);
	virtual ~IPoint(void);
	
	// utility for the hashset
	int			getNext(void);
	void		setNext(int to);
	// misc.
	void		dump(void);
};

// comparator for sorting pixels
class IPointCmpByPos : public MComparator<IPoint> {
public:
	IPointCmpByPos(void) {};
	virtual ~IPointCmpByPos(void) {};
	virtual bool	equals(IPoint const &a, IPoint const &b) const {
		return a._x == b._x && a._y == b._y;
	};
	virtual int	compare(IPoint const &a, IPoint const &b) const {
		if ( a._y < b._y ) return -1;
		if ( a._y > b._y ) return 1;
		if ( a._x < b._x ) return -1;
		if ( a._x > b._x ) return 1;
		return 0;
	};
};

// hash retriever
// the actual hash computation is in the IPoint class constructor
class IPointHashByPos : public MHashor<IPoint> {
public:
	IPointHashByPos(void) {};
	virtual ~IPointHashByPos(void) {};
	
	virtual	bool	equals(IPoint const &a, IPoint const &b) const {
		return a._x == b._x && a._y == b._y;
	};
	virtual	int		hash(IPoint const &a) const {
		return a._hash;
	};
};

#endif /*FPOINT_H_*/
