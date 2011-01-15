/*
 * a 2D polygon
 *
 * this class superclasses Polyline, so a Path to VarotPolygon conversion is:
 * 'path->convert(polygon, precision)'
 *
 * the computation routine is uncross(), which does the uncrossing and creates a polygon
 * according to the operation selected
 *
 * for boolean operations, one needs a set of 'A' paths and a set of 'B' paths; then the
 * result of, say, a difference will be 'area of winding > 0 for the A paths' - 'area of
 * winding > 0 for the B paths'
 *
 * to set the source (for operations needing it), a call to setSource() prior to the
 * convert() is necessary
 */

#ifndef POLYGON_H_
#define POLYGON_H_

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "FPoint.h"
#include "MComparator.h"
#include "MArray.h"
#include "Polyline.h"
#include "Walker.h"

class VarotPolygon;
class Adjacency;
class IntersEdge;
class WindingEdge;
class WindingAdjacency;
class Image;
class Path;
class PathElem;

/*
 * the different operations on polygons
 */
enum {
	winding_evenodd, // even-odd fill
	winding_nonzero, // non-zero fill
	winding_positive, // non-zero positive fill
	winding_OR, // boolean or
	winding_AND, // boolean and
	winding_XOR, // boolean XOR
	winding_DIFF // boolean difference
};

/*
 * internal class, for storing a polygon edge
 */
class FEdge {
public:
	int			_no;
	int			_st, _en;
	float		_stS, _enS;
	int			_group;
	PathElem*	_S;

	FEdge(void);
	FEdge(int st, int en, VarotPolygon *pol);
	FEdge(int st, int en, float stS, float enS, int group, PathElem* S, VarotPolygon *pol);
	virtual ~FEdge(void);
};


class VarotPolygon : public Polyline
{
	friend class IntersEdge;
protected: // for the IntersEdge utility class
	MArray<FPoint>		_pts;
	MArray<FEdge>		_edges;
private:
	MArray<Path*>		_sourcesA; // just for the record
	MArray<Path*>		_sourcesB; // just for the record

	/*
	 * path recomposed after the operation (only computed if asked)
	 * *should* be devoid of self-intersections, but transcribing groups of non-intersecting
	 * edges into curves can potentially reintroduce self-intersections...
	 */
	Path*				_result;

	/*
	 * records the last point on the path currently converted
	 * this is needed because the polygon removes 0-length edges right at the start
	 * but a 'almost 0-length' edge that is omitted still counts as a little increase
	 * in the curve abcissis. since the recomposing into curves expects consecutive
	 * abcissis intervals on the edges, we need to enforce them when 'omitting' an edge
	 */
	float				_lastS;
	PathElem*			_lastOrigin;
	bool				_sourceIsA;

public:
	VarotPolygon();
	VarotPolygon(VarotPolygon const &o);
	virtual ~VarotPolygon();

	/*
	 * sets the path to be converted next, and more specifically, the group it belongs to:
	 * 'A' or 'B'
	 */
	void			setSource(Path* p, bool isA);
	/*
	 * callbacks for the Polyline class
	 * (we trust the base implementation for rounding points)
	 */
	virtual	int		createPoint(FPoint const &p);
	virtual	int		createEdge(int st, int en, float stS, float enS, PathElem* origin);

	/*
	 * rasters the polygon in the mask (coverage delta creation phase)
	 */
	void	toImage(Image *dest);
	/*
	 * makes a svg file with the polygon, for debugging
	 */
	void	toFile(const char* fileName);

	/*
	 * computation
	 * if redoPath = true, a resulting path is computed, and is avalaible with givePath
	 */
	void	uncross(int mode, bool redoPath);
	Path*	givePath(void);

private:
	int		addPt(float x, float y);
	int     addPt(FPoint npt);

	/*
	 * intersection finding
	 */
	void	allIntersections(MArray<IntersEdge> &iEdges);
	void	allIntersections(MArray<IntersEdge> &iEdges, float minX, float maxX, float minY, float maxY);
	void	calcIntersections(MArray<IntersEdge> &iEdges, float minX, float maxX, float minY, float maxY);

	/*
	 * adjacencies finding
	 */
	void	allAdjacencies(MArray<IntersEdge> iEdges, MArray<Adjacency> &adjs);
	/*
	 * computing winding numbers for all edges, and both A and B groups
	 */
	void	calcWindings(MArray<WindingAdjacency> &nadjs, MArray<WindingEdge> &nedges, MArray<IntersEdge> &iEdges);

	/*
	 * misc.
	 */
	void	dump(void);
};

/*
 * storage for an edge during the intersection part
 */
class IntersEdge {
public:
	bool			_downward;	// if both endpoints on the same horizontal line,
	                            // this actually means 'rightwards'
	float			_left, _right, _top, _bottom; // bounding box
	FPoint			_st, _en; // endpoints
	double			_l2; // square of the length
	int				_group;

	IntersEdge(void);
	IntersEdge(int e, VarotPolygon *p);
	virtual ~IntersEdge(void);

	/*
	 * overlap tests
	 */
	bool touches(IntersEdge const &o) const;
	bool touches(MPoint<float> const &o) const;

	/*
	 * intersection tests
	 * returns the intersection point (if it exists) in 'at'
	 */
	bool intersects(IntersEdge const &o, FPoint &at) const;
	bool intersects(FPoint const &o);
};

/*
 * storage for a edge-point adjacency
 */
class Adjacency {
public:
	int			_no;
	int			_pt, _edge;
	double		_abs; // position of the adjacency on the edge

	Adjacency(void) {};
	Adjacency(int pt, int ed, IntersEdge const &ie, FPoint const &ip) {
		_pt = pt;
		_edge = ed;
		_abs = (ie._en - ie._st )|(ip - ie._st);
	};
	virtual ~Adjacency(void) {};
};

/*
 * comparator for sorting the adjacencies by 'edge + position on edge'
 * the goal is to have all points touching an edge in the order of appearance on the edge,
 * ie the startpoint comes first
 */
class AdjacencyCmpByEdge : public MComparator<Adjacency> {
public:
	AdjacencyCmpByEdge(void) {};
	virtual ~AdjacencyCmpByEdge(void) {};
	virtual int	compare(Adjacency const &a, Adjacency const &b) const ;
};

/*
 * utility class for walking on the polygon edges and computing the winding numbers
 */
class WindingWalker : public Walker {
private:
	MArray<IntersEdge>			_inters; // edges of the polygon
	MArray<FPoint>				_pts; // points of the polyogn
	int							_curNum0; // current winding number for group 'A'
	int							_curNum1; // current winding number for group 'B'

public:
	WindingWalker(MArray<WindingEdge> const &ed, MArray<WindingAdjacency> const &ad, MArray<IntersEdge> const &in, MArray<FPoint> const &pt);
	virtual ~WindingWalker();

	/*
	 * callbacks for the polygon walker
	 */
	virtual	int		newConnectedComponent(int startAdj);
	virtual int     circleAtPoint(int nadj, int ptNo, bool clockwise);
	virtual void	postBacktrack(int curAdj, int wNo,  bool clockwise, WindingEdge* curWind);
	virtual void	preAdvanceAtEdge(int curAdj, int wNo,  bool clockwise, WindingEdge* curWind);
};

/*
 * utility class for recomposing the curves from the polygon, using the information about
 * each edge's source (stored on said edges)
 */
class ReformWalker : public Walker {
private:
	Path*						nshape; // result path
	MArray<FPoint>				_pts; // polygon points
	PathElem*					_curSrc; // source of current edge
	float						_startS, _endS; // current abcissis interval on the source
	bool						_inSubpath; // is a subpath currently open?

public:
	ReformWalker(MArray<WindingEdge> const &ed, MArray<WindingAdjacency> const &ad, MArray<FPoint> const &pt);
	virtual ~ReformWalker();

	/*
	 * return the recomposed path
	 * the polygon will not assume ownership of the path, whoever gets it will
	 * be responsible for its disposal
	 */
	Path*			givePath(void);

	/*
	 * callbacks for the walk on the polygon
	 */
	virtual	int		newConnectedComponent(int startAdj);
	virtual int     circleAtPoint(int nadj, int ptNo, bool clockwise);
	virtual void	preBacktrack(int curAdj, int wNo,  bool clockwise, WindingEdge* curWind);
	virtual void	advanceAtPoint(int curAdj, int wNo,  bool clockwise, WindingEdge* curWind);
	virtual void	postAdvanceAtEdge(int curAdj, int wNo,  bool clockwise, WindingEdge* curWind);
};


#endif /*POLYGON_H_*/
