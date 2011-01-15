#include "VarotPolygon.h"
#include "Image.h"
#include "Path.h"
#include "PathElem.h"
	
FEdge::FEdge(void) {
	_no = -1;
}
FEdge::FEdge(int st, int en, VarotPolygon *pol) {
	_st = st;
	_en = en;
	_stS = 0;
	_enS = 1;
	_S = NULL;
	_group = -1;
	_no = -1;
}
FEdge::FEdge(int st, int en, float stS, float enS, int group, PathElem* S, VarotPolygon *pol) {
	_st = st;
	_en = en;
	_stS = stS;
	_enS = enS;
	_S = S;
	_no = -1;
	_group = group;
	_S = S;
}
FEdge::~FEdge(void) {
	
}

VarotPolygon::VarotPolygon()
{
	_result = NULL;
	_lastOrigin = NULL;
}
VarotPolygon::VarotPolygon(VarotPolygon const &o) : _pts(o._pts), _edges(o._edges)
, _sourcesA(o._sourcesA), _sourcesB(o._sourcesB)
{
	_result = NULL;
	_lastOrigin = NULL;
}

VarotPolygon::~VarotPolygon()
{
	// givePath() sets _result to NULL so this only
	// happens if the path was recomposed but never asked for
	if ( _result != NULL ) delete _result;
}

void			VarotPolygon::setSource(Path* p, bool isA) {
	_sourceIsA = isA;
	if ( _sourceIsA )
		_sourcesA.append(p);
	else
		_sourcesB.append(p);
}

/*
 * callbacks of Polyline
 */
int		VarotPolygon::createPoint(FPoint const &p) {
	// this function will (most surely) make duplicate points, but the uncrossing 
	// computation will only keep one
	FPoint npt = p;
	npt._no = _pts.nb();
	return _pts.append(npt);
}
int		VarotPolygon::createEdge(int st, int en, float stS, float enS, PathElem* origin) {
	// get rid of 0-length edges
	if ( st == en ) return -1;
	MPoint<float>	d = _pts[en] - _pts[st];
	if ( d._x == 0 && d._y == 0 ) return -1;
	// edge length > 0 : we keep it
	if ( _lastOrigin != NULL && _lastOrigin == origin ) {
		stS = _lastS;
	}
	_edges.append(FEdge(st, en, stS, enS, (_sourceIsA)?0:1, origin, this));
	_lastOrigin = origin;
	_lastS = enS;
	return _edges.nb() -1;
}

/*
 * adding points, for intersection computations
 */
int		VarotPolygon::addPt(float x, float y) {
	FPoint	npt(x,y);
	npt.round();
	return addPt(npt);
}
int		VarotPolygon::addPt(FPoint npt) {
	npt._no = _pts.nb();
	return _pts.append(npt);
}

/*
 * removal of self-intersections
 */
void	VarotPolygon::uncross(int mode, bool redoPath) {
	if ( _pts.nb() <= 0 ) return;
	// extract all intersections between edges
	MArray<IntersEdge> iEdges;
	allIntersections(iEdges);
	// sort points by position, and only keep one by position
	MArray<int>	bij(_pts.nb(), true);
	for (int i=0;i<_pts.nb();i++) _pts[i]._no = i;
	FPointCmpByPos  ptCmp;
	_pts.sort(0, _pts.nb()-1, ptCmp);
	// bij will store the mapping 'old point index' -> 'new point index'
	int cur = 0;
	bij[_pts[cur]._no] = cur;
	for (int i=1;i<_pts.nb();i++) {
		if ( _pts[i].equals(_pts[cur]) ) {
		} else {
			cur++;
			if ( cur != i ) _pts[cur] = _pts[i];
		}
		bij[_pts[i]._no] = cur;
	}
	_pts.truncate(cur+1);
	// refresh point indexes in the edges
	for (int e=0;e<_edges.nb();e++) {
		_edges[e]._st = bij[_edges[e]._st];
		_edges[e]._en = bij[_edges[e]._en];
	}

	// get all point-edge adjacencies
	MArray<Adjacency>	adjs;
	allAdjacencies(iEdges, adjs);
	// create the edges of the polygon without self-intersections
	// for the moment, it's edge+adjacency representation
	// sort by edge+abcissis
	MArray<int>	adjPos(adjs.nb(),true);
	for (int i=0;i<adjs.nb();i++) adjs[i]._no = i;
	AdjacencyCmpByEdge  adjEdgeCmp;
	adjs.sort(0, adjs.nb()-1, adjEdgeCmp);
	for (int i=0;i<adjs.nb();i++) adjPos[adjs[i]._no] = i;
	// create edges
	// the new adjacencies are set up with an interpolated abcissis on the curve,
	// derived from the position on the edge and the start/end abcissis of the edge
	// (this is an approximation)
	MArray<WindingEdge>			nedges;
	MArray<WindingAdjacency>	nadjs;
	int	lastEdge = -1;
	int lastPoint = -1;
	double	curEdgeLength = 0;
	double	lastAbs = 0;
	for (int a=0;a<adjs.nb();a++) {
		int		nPoint = adjs[a]._pt;
		int		nEdge = adjs[a]._edge;
		double	nAbs = adjs[a]._abs;
		int			edgeGrp = _edges[nEdge]._group;
		PathElem*	edgeSrc = _edges[nEdge]._S;
		float		edgeAbsSt = _edges[nEdge]._stS;
		float		edgeAbsEn = _edges[nEdge]._enS;
		if ( nEdge == lastEdge ) {
			// add a new edge and its 2 adjacencies
			nAbs /= curEdgeLength;
			nAbs = nAbs * edgeAbsEn + (1-nAbs) * edgeAbsSt;
			int	curA = nadjs.nb();
			int curE = nedges.nb();
			nedges.append(WindingEdge(curA, curA +1, lastPoint, nPoint, edgeGrp, edgeSrc, _pts[nPoint] - _pts[lastPoint]));
			nadjs.append(WindingAdjacency(lastPoint, curE, lastAbs ));
			nadjs.append(WindingAdjacency(nPoint, curE, nAbs));
		} else {
			// move to a new edge (of the original polygon)
			FPoint	edgeDir = iEdges[nEdge]._en - iEdges[nEdge]._st;
			curEdgeLength = edgeDir|edgeDir;
			nAbs /= curEdgeLength;
			nAbs = nAbs * edgeAbsEn + (1-nAbs) * edgeAbsSt;
		}
		lastEdge = nEdge;
		lastPoint = nPoint;
		lastAbs = nAbs;
	}
	// sort adjacencies by point+clockwise order
	MArray<int>	nadjPos(nadjs.nb(), true);
	for (int i=0;i<nadjs.nb();i++) nadjs[i]._no = i;
	AdjacencyCmpByPoint  adjPointCmp( &nedges);
	nadjs.sort(0, nadjs.nb()-1, adjPointCmp);
	for (int i=0;i<nadjs.nb();i++) nadjPos[nadjs[i]._no] = i;
	// refresh adjacencies indexes
	for (int e=0;e<nedges.nb();e++) {
		nedges[e]._stA = nadjPos[nedges[e]._stA];
		nedges[e]._enA = nadjPos[nedges[e]._enA];
	}
	// compute winding numbers
	// a sweepline algorithm would be able to compute winding numbers for each edge
	// as it is created, but in this algorithm this is not possible because intersections 
	// are computed without particular order.
	WindingWalker	windingComp(nedges, nadjs, iEdges, _pts);
	windingComp.walk(true);

	// with the winding numbers, it is possible to decide which edge stays
	// and which one goes
	// the (intermediary) edges are also reset to _visited = false for the edges that
	// stay, in case we need to do a second eulerian tour for the recomposition of the
	// curve
	_edges.truncate(0);
	for (int i=0;i<_pts.nb();i++) _pts[i]._no = 0;
	for (int i=0;i<nadjs.nb();i++) nadjs[i]._visited = false;
	for (int i=0;i<nedges.nb();i++) {
		nedges[i]._visited = true;
		nedges[i]._prev = NULL;
		bool	keep = false;
		bool	revert = false;
		if ( mode == winding_evenodd ) {
			int	lMod = nedges[i]._left0 % 2;
			int	rMod = nedges[i]._right0 % 2;
			if ( lMod < 0 ) lMod = -lMod;
			if ( rMod < 0 ) rMod = -rMod;
			if ( lMod == 0 && rMod == 1 ) {
				keep = true;
			} else if ( lMod == 1 && rMod == 0 ) {
				keep = true;
				revert = true;
			}
		} else if ( mode == winding_nonzero ) {
			if ( nedges[i]._left0 == 0 && nedges[i]._right0 == 1 ) {
				keep = true;
			} else if ( nedges[i]._left0 == -1 && nedges[i]._right0 == 0 ) {
				keep = true;
				revert = true;
			}
		} else if ( mode == winding_positive ) {
			if ( nedges[i]._left0 == 0 && nedges[i]._right0 == 1 ) {
				keep = true;
			}
		} else if ( mode == winding_OR ) {
			int lTot = ((nedges[i]._left0 >= 1)?1:0)+((nedges[i]._left1 >= 1)?1:0);
			int rTot = ((nedges[i]._right0 >= 1)?1:0)+((nedges[i]._right1 >= 1)?1:0);
			if ( lTot == 0 && rTot >= 1 ) {
				keep = true;
			} else if ( lTot >= 1 && rTot == 0 ) {
				keep = true;
				revert = true;
			}			
		} else if ( mode == winding_AND ) {
			int lTot = ((nedges[i]._left0 >= 1)?1:0)+((nedges[i]._left1 >= 1)?1:0);
			int rTot = ((nedges[i]._right0 >= 1)?1:0)+((nedges[i]._right1 >= 1)?1:0);
			if ( lTot == 1 && rTot == 2 ) {
				keep = true;
			} else if ( lTot == 2 && rTot == 1 ) {
				keep = true;
				revert = true;
			}			
		} else if ( mode == winding_XOR ) {
			int lTot = ((nedges[i]._left0 >= 1)?1:0)+((nedges[i]._left1 >= 1)?1:0);
			int rTot = ((nedges[i]._right0 >= 1)?1:0)+((nedges[i]._right1 >= 1)?1:0);
			if ( lTot == 2 ) lTot = 0;
			if ( rTot == 2 ) rTot = 0;
			if ( lTot == 0 && rTot == 1 ) {
				keep = true;
			} else if ( lTot == 1 && rTot == 0 ) {
				keep = true;
				revert = true;
			}			
		} else if ( mode == winding_DIFF ) {
			int lTot = ((nedges[i]._left0 >= 1)?1:0)-((nedges[i]._left1 >= 1)?1:0);
			int rTot = ((nedges[i]._right0 >= 1)?1:0)-((nedges[i]._right1 >= 1)?1:0);
			if ( lTot <= 0 && rTot == 1 ) {
				keep = true;
			} else if ( lTot == 1 && rTot <= 0 ) {
				keep = true;
				revert = true;
			}			
		}

		if ( keep ) {
			nedges[i]._visited = false;
			if ( revert ) {
				int sw;
				sw = nedges[i]._stA;nedges[i]._stA = nedges[i]._enA;nedges[i]._enA = sw;
				sw = nedges[i]._left0;nedges[i]._left0 = nedges[i]._right0;nedges[i]._right0 = sw;
				sw = nedges[i]._left1;nedges[i]._left1 = nedges[i]._right1;nedges[i]._right1 = sw;
			}
			WindingAdjacency	stA = nadjs[nedges[i]._stA];
			WindingAdjacency	enA = nadjs[nedges[i]._enA];
			_pts[stA._pt]._no ++;
			_pts[enA._pt]._no ++;
			_edges.append(FEdge(stA._pt, enA._pt, this));
		} else {
			nadjs[nedges[i]._stA]._visited = true;
			nadjs[nedges[i]._enA]._visited = true;
		}
	}

	// recompose the path
	if ( redoPath ) {
		ReformWalker	reformComp(nedges, nadjs, _pts);
		reformComp.walk(false);
		_result = reformComp.givePath();
	} else {
		_result = NULL;
	}
}

Path*	VarotPolygon::givePath(void) {
	Path*	res = _result;
	_result = NULL;
	return res;
}

int	AdjacencyCmpByEdge::compare(Adjacency const &a, Adjacency const &b) const {
	if ( a._edge < b._edge ) return -1;
	if ( a._edge > b._edge ) return 1;
	if ( a._abs < b._abs ) return -1;
	if ( a._abs > b._abs ) return 1;
	return 0;
}

/*
 * rasterization: simply pass each edge to the edge-flag computations
 */
void	VarotPolygon::toImage(Image *dest) {
	for (int i=0;i<_edges.nb();i++) {
		dest->addDerivative(_pts[_edges[i]._st]._x, _pts[_edges[i]._st]._y
				           ,_pts[_edges[i]._en]._x, _pts[_edges[i]._en]._y
				           , 0, dest->_sizeV-1);
	}
}

void	VarotPolygon::dump(void) {
}
