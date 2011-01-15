/*
 * curve -> polygon conversion
 * 
 * the class receives moveto/lineto/close commands during the path to polline conversion,
 * and must act accordingly
 * the basic behavior is to round the points and call the point/edge creation subroutines.
 * the polygon class override these, while the image class overrides directly the moveto and
 * lineto, since it doesn't care for rounding
 */

#ifndef POLYLINE_H_
#define POLYLINE_H_

#include <math.h>
#include "MPoint.h"
#include "FPoint.h"

class PathElem;

class Polyline
{
private:
	FPoint			_firstPt; // first point of the subpath
	FPoint			_firstRoundedPt; // first point of the subpath, rounded
	int				_firstIndex;
                                 // -1 acts as 'no point'
	FPoint			_lastPt; // last point of the subpath
	FPoint			_lastRoundedPt; // last point of the subpath, rounded
	int				_lastIndex; // index of the last point, as returned by createPoint()
                                // -1 acts as 'no point'
	
public:
	Polyline();
	virtual ~Polyline();
	
	/*
	 * callbacks called during the conversion
	 */
	virtual void	moveto(MPoint<float> const &p, PathElem* origin);
	virtual void	lineto(MPoint<float> const &p, float stS, float enS, PathElem* origin);
	virtual	void	close(PathElem* origin);

	/*
	 * atomic creation routines (empty in the base implementation)
	 */
	virtual	int		createPoint(FPoint const &p);
	virtual	int		createEdge(int st, int en, float stS, float enS, PathElem* origin);
};

#endif /*POLYLINE_H_*/
