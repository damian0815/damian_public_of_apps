/*
 * approximation of a sequence of points by a curve
 * 
 * the class is first filled with points via addPoint(), then simplify() is called
 * which returns a path
 * 
 * the fitting is kind of 'brute-force' since it's a dynamic programming method (ie:
 * it's n^2 where n is the number of points). the goal is to find a curve either:
 * - where the average distance of points to the curve is bounded by delta, and
 *   the curve is as small as possible
 * - where the number of cubic patches in the curve is bounded by nbElem and the 
 *   average distance of points to the curve is as small as possible
 */

#ifndef POINTSEQUENCE_H_
#define POINTSEQUENCE_H_

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "MArray.h"
#include "MPoint.h"

class Path;

/*
 * storage class: a point in the sequence we wish to convert to a curve
 */
class FitPoint {
public:
	MPoint<double>	_pt; // the point itself
	MPoint<double>	_q; // temp value for the fitter
	double			_l; // distance to the prev point
	double			_t; // abcissis on the estimated curve
	int				_type; // nto used yet
	
	FitPoint(void) : _pt(), _q() {_l = 0; _t = 0; _type = 0;};
	FitPoint(MPoint<double> const &p) : _pt(p), _q() { _l = 0; _t = 0; _type = 0;};
	virtual ~FitPoint(void) {};
};

/*
 * 
 */
class PointSequence
{
private:
	MArray<FitPoint>	_pts;
	
	// result of an attemp to fit points: a cubic bezier patch
	MPoint<double>		_P0;
	MPoint<double>		_P1;
	MPoint<double>		_P2;
	MPoint<double>		_P3;
	double				_delta;
public:
	PointSequence();
	virtual ~PointSequence();
	
	/*
	 * append a point to the sequence
	 */
	void	addPoint(MPoint<float> const &p);
	
	/*
	 * simplify to a single patch
	 */
	Path*	simplify(void);
	/*
	 * find a path with at most nbElem patches, which minimizes the average 
	 * distance of points to it
	 */
	Path*	simplify(int nbElem, bool approx);
	/*
	 * find a path where the average distance to it is at most delta, and with
	 * minimum number of bezier patches
	 */
	Path*	simplify(double delta, bool approx);
	/*
	 * combined version: if nbElem < 0, then it's simplify(delta, approx),
	 * otherwise it's simplify(nbElem, approx)
	 */
	Path*	simplify(int nbElem, double delta, bool approx);
	
private:
	/*
	 * compute a first estimation of the abcisses of points on the curve
	 */
	void	initPositions(int stP, int enP);
	/*
	 * improve abcissis a bit with a newton-rhapson iteration on tthe derivative
	 * of the square of the distance to the curve
	 */
	void	nudgePositions(int stP, int enP);
	/*
	 * recompute the sum of distances to the curve
	 */
	void	refreshDelta(int stP, int enP);
	/*
	 * fit a bezier patch through the points
	 */
	void	calcControlPoints(int stP, int enP);
	
	/*
	 * for debugging
	 */
	void	toFile(const char* fileName);
};

#endif /*POINTSEQUENCE_H_*/
