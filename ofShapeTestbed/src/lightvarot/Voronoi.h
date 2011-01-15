/*
 * building the voronoi diagram of a set of points
 * 
 * the goal is to return a path representing all the faces in the graph. so a
 * face is a subpath of the result
 * 
 * to avoid dealing with infinite faces, we add 4 points outside the bounding 
 * box of the input points: this way, the faces of the input points are all 
 * finite
 */
#ifndef VORONOI_H_
#define VORONOI_H_

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "MArray.h"
#include "MPoint.h"
#include "MComparator.h"

class Path;
class VoronoiArc;

/*
 * an event in fortune's algorithm:
 * - if _ends == NULL, then it's an input point
 * - if _ends != NULL, then this event ends the beachline arc '_ends'
 */
class VoronoiEvent {
public:
	VoronoiArc*		_ends;
	MPoint<double>	_pt;
	int				_ptNo;
	double			_radius; // radius of a circle event, to recover the 
	                         // circle center (event is the bottommost point
	                         // of the circle)
	
	VoronoiEvent(void) : _pt() {_ends = NULL;};
	VoronoiEvent(VoronoiEvent const &o) : _pt(o._pt) {_ends = o._ends;_ptNo = o._ptNo;};
	VoronoiEvent(MPoint<double> const &pt, int no) : _pt(pt) {_ends = NULL;_ptNo = no;};
	VoronoiEvent(MPoint<double> const &pt, VoronoiArc* of,double rad) : _pt(pt) {_radius = rad;_ends = of;};
	virtual ~VoronoiEvent(void) {};
	
	//void	dump(void) {printf("[%X: %f %f %X] ", this, _pt._x, _pt._y,_ends);};
};

/*
 * a parabolic arc in the beachline
 */
class VoronoiArc {
public:
	VoronoiEvent*	_end; // event ending this arc, if it exists
	MPoint<double>	_pt;
	int				_ptNo;
	VoronoiArc*		_prev;
	VoronoiArc*		_next;
	double			_left, _right;
	
	VoronoiArc(void) : _pt() {_end = NULL;_prev=NULL;_next=NULL;};
	VoronoiArc(MPoint<double> const &pt, int no) : _pt(pt) {_ptNo = no; _end = NULL;_prev=NULL;_next=NULL;};
	virtual ~VoronoiArc(void) {};
};

/*
 * utility for the graph building: face/point adjacency
 */
class VoronoiAdjacency {
public:
	int				_ptNo;
	MPoint<double>	_pt;
	MPoint<double>	_center;
	
	VoronoiAdjacency(void) {};
	VoronoiAdjacency(VoronoiAdjacency const &o) : _pt(o._pt), _center(o._center) {_ptNo = o._ptNo;};
	VoronoiAdjacency(int no, MPoint<double> const &pt, MPoint<double> const &center) : _pt(pt), _center(center) {_ptNo = no;};
	virtual ~VoronoiAdjacency(void) {};
	
};

/*
 * comparator for the binary heap
 */
class VoronoiEventByPos : public MComparator<VoronoiEvent*> {
public:
	VoronoiEventByPos(void) {};
	virtual ~VoronoiEventByPos(void) {};
	virtual bool	equals(VoronoiEvent* const &a, VoronoiEvent* const &b) const ;
	virtual int		compare(VoronoiEvent* const &a, VoronoiEvent* const &b) const ;
};

/*
 * comparator for the adjacencies: the goal is to sort by face number and then
 * sort the points of a face in clockwise order
 */
class VoronoiAdjacencyByAng : public MComparator<VoronoiAdjacency> {
public:
	VoronoiAdjacencyByAng(void) {};
	virtual ~VoronoiAdjacencyByAng(void) {};
	virtual int		compare(VoronoiAdjacency const &a, VoronoiAdjacency const &b) const ;
};

/*
 * computation class
 */
class Voronoi
{
private:
	MArray<MPoint<double> >	_pts;
	VoronoiArc*				_first; // the beacline is a doubly-linked list of arcs
	VoronoiArc*				_last;
	
public:
	Voronoi();
	virtual ~Voronoi();

	/*
	 * add a point to the set of input points
	 */
	void	addPoint(MPoint<double> const &pt);
	/*
	 * returns the voronoi diagram of the set of points
	 */
	Path*	graph(void);

	/*
	 * recomputes the x-spans of the arcs in the beachline ([_left;_right])
	 * for the sweep position y
	 */
	void	refreshXs(double y);
	/*
	 * inserts an arc in the doubly linked list, between 'after' and 'before'
	 */
	void	insert(VoronoiArc* arc, VoronoiArc* after, VoronoiArc* before);
	/*
	 * tries to compute the circle event that ends this arc, if it exists
	 */
	VoronoiEvent*	endOf(VoronoiArc* arc);
};

#endif /*VORONOI_H_*/
