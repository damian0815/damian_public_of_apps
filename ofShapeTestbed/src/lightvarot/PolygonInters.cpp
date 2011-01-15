#include "VarotPolygon.h"

/*
 * temporary storage for intersection computation
 * 
 * this helper class is mainly used to hold bounding box of each edge
 * to make it possible to do quick intersection tests
 */
IntersEdge::IntersEdge(void) : _st(), _en() {
	_downward = true;
	_left = _right = _top = _bottom = 0;
}
IntersEdge::IntersEdge(int e, VarotPolygon *p) : _st(p->_pts[p->_edges[e]._st])
                                          , _en(p->_pts[p->_edges[e]._en]) {
	FPoint	l = _en - _st;
	_group = p->_edges[e]._group;
	_downward = ( l._y > 0 || ( l._y == 0 && l._x > 0 ) );
	_l2 = l | l;
	if ( _st._x < _en._x ) {
		_left = _st._x;
		_right = _en._x;
	} else {
		_left = _en._x;
		_right = _st._x;
	}
	if ( _st._y < _en._y ) {
		_top = _st._y;
		_bottom = _en._y;
	} else {
		_top = _en._y;
		_bottom = _st._y;
	}
}
IntersEdge::~IntersEdge(void) {
	
}
/*
 * return true if this edge could overlap with the other one
 * 
 * so if bounding box intersect on their borders only, the only edge
 * intersections that can occur is when edges are colinear. but in that case
 * the adjacency step will find these intersections
 * 
 * so only the inside of the bounding boxes needs to overlap
 */
bool IntersEdge::touches(IntersEdge const &o) const {
	if ( _left >= o._right ) return false;
	if ( _right <= o._left ) return false;
	if ( _top >= o._bottom ) return false;
	if ( _bottom <= o._top ) return false;
	return true;
}
/*
 * return true if edge could be adjacent with the edge
 */
bool IntersEdge::touches(MPoint<float> const &o) const {
	if ( _left > o._x ) return false;
	if ( _right < o._x ) return false;
	if ( _top > o._y ) return false;
	if ( _bottom < o._y ) return false;
	return true;	
}
/*
 * test whether 2 edges intersects
 */
bool IntersEdge::intersects(IntersEdge const &o, FPoint &at) const {
	// because we check sides of the points of the 'other' edge, it's
	// wiser to use the shortest edge to check sides
	if ( _l2 > o._l2 ) return o.intersects(*this,at);
	FPoint	d = _en - _st;
	FPoint	ss = o._st - _st;
	FPoint	se = o._en - _st;
	double	css = d ^ ss;
	double	cse = d ^ se;
	// first: test whether both endpoints of the other edges are on the same side
	// if not, then the line supporting the edge cross the other edge
	if ( css * cse < 0 ) {
		FPoint	i = (css * o._en - cse * o._st) / (css - cse);
		i.round();
		// if the intersection point lies inside the bounding box, then it is
		// on the edge
		if ( touches(i) ) {
			at = i;
			return true;
		}
	}
	return false;
}
/*
 * tests edge-point adjacency
 * 
 * this function is always called after a touches(), so point is inside the bounding box
 * of the edge
 * 
 * an edge is adjacent to a point if it passes through the pixel centered one this point
 * and of size 1/2 the rouding step. since the pixels need to cover all the plane, the 
 * upper and left borders of the pixels are considered 'inside', while the bottom and right
 * borders are 'outside'
 */
bool IntersEdge::intersects(FPoint const &o) {
	if ( _left == _right ) {
		return ( o._y > _top && o._y < _bottom );
	} else if ( _top == _bottom ) {
		return ( o._x > _left && o._x < _right );
	} else {
		if ( o.equals(_st) || o.equals(_en) ) return false;
		float	r = ldexpf(0.5f, -rounding_power);
		FPoint	d = _en - _st;
		FPoint	l = o - _st;
		// the 4 corners of the pixel
		FPoint	ul (l._x - r, l._y - r);
		FPoint	ur (l._x + r, l._y - r);
		FPoint	ll (l._x - r, l._y + r);
		FPoint	lr (l._x + r, l._y + r);
		// test sides of each pixel's corner
		double	cul = d ^ ul;
		double	cur = d ^ ur;
		double	cll = d ^ ll;
		double	clr = d ^ lr;
		int nbM = 0, nbP = 0;
		if ( cul <= 0 ) nbM++; else nbP++;
		if ( cur < 0 ) nbM++; else nbP++;
		if ( cll < 0 ) nbM++; else nbP++;
		if ( clr < 0 ) nbM++; else nbP++;
		// if not all corners are on the same side, then the edge passes through the pixel
		return (nbP != 0 && nbM != 0);
	}
}
/*
 * computes all edges intersections and add the points to the polyogn
 * 
 * to be a *little* smart, a divide-and-conquer strategy is used
 * the plane is recursively splitted in quads, until splitting doesn't
 * reduce the number of edge pairs to test by a significant margin. then
 * the pairs a brutally tested
 */
void	VarotPolygon::allIntersections(MArray<IntersEdge> &iEdges) {
	if ( _edges.nb() <= 0 ) return;
	for (int e=0;e<_edges.nb();e++) iEdges.append(IntersEdge(e,this));
	float minX = iEdges[0]._st._x;
	float minY = iEdges[0]._st._y;
	float maxX = minX, maxY = minY;
	for (int i=1;i<iEdges.nb();i++) {
		float px, py;
		px = iEdges[i]._st._x;
		py = iEdges[i]._st._y;
		if ( px < minX ) minX = px;
		if ( px > maxX ) maxX = px;
		if ( py < minY ) minY = py;
		if ( py > maxY ) maxY = py;
		px = iEdges[i]._en._x;
		py = iEdges[i]._en._y;
		if ( px < minX ) minX = px;
		if ( px > maxX ) maxX = px;
		if ( py < minY ) minY = py;
		if ( py > maxY ) maxY = py;
	}
	allIntersections(iEdges, minX, maxX + 1, minY, maxY + 1);
}
/*
 * recursive intersections computation
 */
void	VarotPolygon::allIntersections(MArray<IntersEdge> &iEdges, float minX, float maxX, float minY, float maxY) {
	if ( iEdges.nb() <= 0 ) return;
	if ( iEdges.nb() <= 10 ) { // not many edges -> brute force
		calcIntersections(iEdges, minX, maxX, minY, maxY);
		return;
	}
	FPoint	mid = 0.5 * (FPoint(minX, minY) + FPoint(maxX, maxY));
	mid.round();
	float midX = mid._x, midY = mid._y;
	// make arrays for the edges touching the upper left quad, upper right, etc
	MArray<IntersEdge>	ul;
	MArray<IntersEdge>	ur;
	MArray<IntersEdge>	lr;
	MArray<IntersEdge>	ll;
	// count the number of edges winding up in a single quand
	// if this number stays low, then the split didn't split the workload much
	int					alone = 0;
	for (int i=0;i<iEdges.nb();i++) {
		int	stQ = -1;
		if ( iEdges[i]._st._x < midX ) {
			if ( iEdges[i]._st._y < midY ) {
				stQ = 0;
			} else {
				stQ = 2;
			}
		} else {
			if ( iEdges[i]._st._y < midY ) {
				stQ = 1;
			} else {
				stQ = 3;
			}
		}
		int	enQ = -1;
		if ( iEdges[i]._en._x < midX ) {
			if ( iEdges[i]._en._y < midY ) {
				enQ = 0;
			} else {
				enQ = 2;
			}
		} else {
			if ( iEdges[i]._en._y < midY ) {
				enQ = 1;
			} else {
				enQ = 3;
			}
		}
		if ( stQ == enQ ) {
			alone ++;
			if ( stQ == 0 ) ul.append(iEdges[i]);
			if ( stQ == 1 ) ur.append(iEdges[i]);
			if ( stQ == 2 ) ll.append(iEdges[i]);
			if ( stQ == 3 ) lr.append(iEdges[i]);
		} else if ( stQ <= 1 && enQ <= 1 ) {
			ul.append(iEdges[i]);
			ur.append(iEdges[i]);
		} else if ( stQ > 1 && enQ > 1 ) {
			ll.append(iEdges[i]);
			lr.append(iEdges[i]);
		} else if ( stQ%2 == 0 && enQ%2 == 0 ) {
			ul.append(iEdges[i]);
			ll.append(iEdges[i]);
		} else if ( stQ%2 == 1 && enQ%2 == 1 ) {
			ur.append(iEdges[i]);
			lr.append(iEdges[i]);
		} else {
			ul.append(iEdges[i]);
			ur.append(iEdges[i]);
			ll.append(iEdges[i]);
			lr.append(iEdges[i]);
		}
	}
	if ( alone * 4 < iEdges.nb() ) {
		// not enough splitting of edge-pairs, little hope that further spitting
		// will make thing better -> bite the bullet and test all pairs
		calcIntersections(iEdges, minX, maxX, minY, maxY);
	} else {
		allIntersections(ul, minX, midX, minY, midY);
		allIntersections(ur, midX, maxX, minY, midY);
		allIntersections(ll, minX, midX, midY, maxY);
		allIntersections(lr, midX, maxX, midY, maxY);
	}
}
/*
 * brute force intersection computation
 */
void	VarotPolygon::calcIntersections(MArray<IntersEdge> &iEdges, float minX, float maxX, float minY, float maxY) {
	for (int i=0;i<iEdges.nb();i++) {
		for (int j=i+1;j<iEdges.nb();j++) {
			if ( iEdges[i].touches(iEdges[j]) ) {
				FPoint at;
				if ( iEdges[i].intersects(iEdges[j],at) ) {
					if ( at._x >= minX && at._x < maxX 
					  && at._y >= minY && at._y < maxY )
						addPt(at);
				}
			}
		}
	}
}
/*
 * edge-point adjacencies computation
 * 
 * (mostly brute force)
 */
void	VarotPolygon::allAdjacencies(MArray<IntersEdge> iEdges, MArray<Adjacency> &adjs) {
	for (int i=0;i<_edges.nb();i++) {
		int pst = _edges[i]._st, pen = _edges[i]._en;
		if ( pst > pen ) {int sw=pst;pst=pen;pen=sw;}
		adjs.append(Adjacency(_edges[i]._st, i, iEdges[i],  iEdges[i]._st));
		adjs.append(Adjacency(_edges[i]._en, i, iEdges[i],  iEdges[i]._en));
		// since point indexes are sorted, the points adjacent to this edge
		// must lie in the [pst,pen] interval
		// this reduces a bit the number of tests
		// only gotcha: if several points are on the same line, they may
		// not be in [pst,pen] but still in the adjacencies
		// so it's necessary to test these points
		for (int j=pst+1;j<pen;j++) {
			if ( iEdges[i].touches(_pts[j]) ) {
				if ( iEdges[i].intersects(_pts[j]) ) {
					adjs.append(Adjacency(j, i, iEdges[i],  _pts[j]));
				}
			}
		}
		float	psty = iEdges[i]._top;
		float	peny = iEdges[i]._bottom;
		for (int j=pst-1;j>=0 && _pts[j]._y == psty;j--) {
			if ( iEdges[i].touches(_pts[j]) ) {
				if ( iEdges[i].intersects(_pts[j]) ) {
					adjs.append(Adjacency(j, i, iEdges[i],  _pts[j]));
				}
			}
		}
		for (int j=pen+1;j<_pts.nb() && _pts[j]._y == peny;j++) {
			if ( iEdges[i].touches(_pts[j]) ) {
				if ( iEdges[i].intersects(_pts[j]) ) {
					adjs.append(Adjacency(j, i, iEdges[i],  _pts[j]));
				}
			}
		}
	}
}
