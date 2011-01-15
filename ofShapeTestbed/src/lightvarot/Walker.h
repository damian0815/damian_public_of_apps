/*
 * utility class for walking on the edges of an UNCROSSED polygon
 * 
 * the assumptions made by this class on the polyogn it's given are:
 * - eulerian: otherwise the polygon is a non-closed shape, and inside/outside cannot be
 *   defined
 * - planar: equivalent to 'without self-intersections'
 * 
 * the class does a standard eulerian tour search, and has callbacks for the important events:
 * - move along an edge
 * - cross a point (ie change edge)
 * - backtrack to a previously visited point
 * 
 * the polygon is not stored as a edge/point pair, but as edge/adjacency, so there are 2 
 * adjacencies by edge.
 */

#ifndef WALKER_H_
#define WALKER_H_

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "FPoint.h"
#include "MComparator.h"
#include "MArray.h"

class PathElem;

/*
 * storage for a edge-point adjacency in the uncrossed graph
 */
class WindingAdjacency {
public:
	int			_no; // number, for updating indexes after sorts
	int			_pt, _edge; // definition of the adjacency
	float		_S; // abcissis of this point on the curve from which the edge came
	bool		_visited; // has been visited by the eulerian tour
	
	WindingAdjacency(void) {};
	WindingAdjacency(int pt, int ed, float S) {
		_pt = pt;
		_edge = ed;
		_visited = false;
		_S = S;
	};
	virtual ~WindingAdjacency(void) {};
};

/*
 * storage for an edge, with all kinds of info needed during the eulerian tours for
 * winding computation and path recomposition
 */
class WindingEdge {
public:
	int				_no; // for reindexing after sorts
	int				_stA, _enA; // start/end adjacencies
	int				_stP, _enP; // start/end points (in the polygon)
	int             _visited; // has been visited by the eulerian tour
	WindingEdge*	_prev; // previous edge on the tour
	int				_left0, _right0; // left/right winding number w.r.t. the 'A' group
	int				_left1, _right1; // left/right winding number w.r.t. the 'B' group
	int				_group; // 0 = 'A' group, 1 = 'B' group
	PathElem*		_orig; // curve from which this edge originated
	FPoint			_dir; // edge as a vector
	
	WindingEdge(void) {};
	WindingEdge(int stA, int enA, int stP, int enP, int group, PathElem* orig, FPoint const &dir) : _dir(dir) {
		_orig = orig;
		_group = group;
		_stA = stA; _enA = enA;
		_stP = stP; _enP = enP;
		_prev = NULL;
		_visited=false;
	};
	virtual ~WindingEdge(void) {};
	
	/*
	 * helper function: return the adjacency which is not 'adj'
	 */
	int		other(int adj) ;
	/*
	 * helper function: return the adjacency which doesn't correspond to the point 'pt'
	 */
	int		otherFromPoint(int pt) ;
	/*
	 * sets the winding numbers and the previous edge on the tour
	 * the values for group 'A' and 'B' (val0 and val1 respectively) are given at 
	 * adjacency 'at'. if 'left'=true, then the values are those of the region on the
	 * left of the edge at this point
	 */
	void	setWinding(int at, bool left, int val0, int val1, WindingEdge* from) ;
	/*
	 * winding number retrival for both groups, at adjacency 'at'.
	 * if 'left'=true, then the value is that of the left side of the edge
	 */
	int		getWinding0(int at, bool left) ;
	int		getWinding1(int at, bool left) ;
	
};

/*
 * comparator for sorting the adjacencies by point+clockwise order+outgoing edge first+edge index
 */
class AdjacencyCmpByPoint : public MComparator<WindingAdjacency> {
public:
	MArray<WindingEdge>*	_edges; // store the edge array to access edge vectors
	
	AdjacencyCmpByPoint(void) {};
	AdjacencyCmpByPoint( MArray<WindingEdge>* edges) {_edges = edges;};
	virtual ~AdjacencyCmpByPoint(void) {};
	virtual int	compare(WindingAdjacency const &a, WindingAdjacency const &b) const ;
};

/*
 * eulerian tour computer
 */
class Walker {
public:
	MArray<WindingEdge>			_edges;
	MArray<WindingAdjacency>	_adjs;
		
	Walker(MArray<WindingEdge> const &ed, MArray<WindingAdjacency> const &ad);
	virtual ~Walker();
	
	/*
	 * compute the tour
	 * if clockwise = true, then the rotation at each point is clockwise
	 */
	void			walk(bool clockwise);

	/*
	 * callbacks for:
	 * - subpath initiation
	 * - rotation at a point
	 * - (pre/post) backtracking in the tour
	 * - crossing a point to a next edge
	 * - (pre/post) moving along an edge
	 */
	virtual	int		newConnectedComponent(int startAdj);
	virtual int     circleAtPoint(int nadj, int ptNo, bool clockwise);
	virtual void	preBacktrack(int curAdj, int wNo,  bool clockwise, WindingEdge* curWind);
	virtual void	postBacktrack(int curAdj, int wNo,  bool clockwise, WindingEdge* curWind);
	virtual void	advanceAtPoint(int curAdj, int wNo,  bool clockwise, WindingEdge* curWind);
	virtual void	preAdvanceAtEdge(int curAdj, int wNo,  bool clockwise, WindingEdge* curWind);
	virtual void	postAdvanceAtEdge(int curAdj, int wNo,  bool clockwise, WindingEdge* curWind);
};

#endif /*WALKER_H_*/
