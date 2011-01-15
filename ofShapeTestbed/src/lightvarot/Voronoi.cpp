#include "Voronoi.h"
#include "Path.h"
#include "MArray.h"
#include "MHeap.h"

int		VoronoiAdjacencyByAng::compare(VoronoiAdjacency const &a, VoronoiAdjacency const &b) const {
	if ( a._ptNo < b._ptNo ) return -1;
	if ( a._ptNo > b._ptNo ) return 1;
	
	MPoint<double>	ad = a._pt - a._center;
	MPoint<double>	bd = b._pt - b._center;
	int aq = 0, bq = 0;
	if ( ad._y < 0 ) {
		if ( ad._x < 0 ) {
			aq = 2;
		} else {
			aq = 3;
		}
	} else {
		if ( ad._x < 0 ) {
			aq = 1;
		} else {
			aq = 0;
		}
	}
	if ( bd._y < 0 ) {
		if ( bd._x < 0 ) {
			bq = 2;
		} else {
			bq = 3;
		}
	} else {
		if ( bd._x < 0 ) {
			bq = 1;
		} else {
			bq = 0;
		}
	}
	if ( aq < bq ) return -1;
	if ( aq > bq ) return 1;
	double	cr = ad ^ bd;
	if ( cr > 0 ) return -1;
	if ( cr < 0 ) return 1;
	return 0;
}

bool	VoronoiEventByPos::equals(VoronoiEvent* const &a, VoronoiEvent* const &b) const {
	return (a==b);
}
int	VoronoiEventByPos::compare(VoronoiEvent* const &a, VoronoiEvent* const &b) const {
	if ( a->_pt._y > b->_pt._y ) return 1;
	if ( a->_pt._y < b->_pt._y ) return -1;
	if ( a->_pt._x > b->_pt._x ) return 1;
	if ( a->_pt._x < b->_pt._x ) return -1;
	return 0;
}

Voronoi::Voronoi() : _pts()
{
}

Voronoi::~Voronoi()
{
}

void	Voronoi::addPoint(MPoint<double> const &pt) {
	_pts.append(pt);
}
Path*	Voronoi::graph(void) {
	Path*	res = new Path();
	if ( _pts.nb() <= 0 ) return res;
	if ( _pts.nb() == 1 ) return res;
	
	// compute the bounding box
	double	left = _pts[0]._x;
	double	right = _pts[0]._x;
	double	top = _pts[0]._y;
	double	bottom = _pts[0]._y;
	for (int i=1;i<_pts.nb();i++) {
		if ( _pts[i]._x < left ) left = _pts[i]._x;
		if ( _pts[i]._x > right ) right = _pts[i]._x;
		if ( _pts[i]._y < top ) top = _pts[i]._y;
		if ( _pts[i]._y > bottom ) bottom = _pts[i]._y;
	}
	double	centerH = 0.5 * (left + right);
	double	centerV = 0.5 * (top + bottom);
	double	sizeH = right - left;
	double	sizeV = bottom - top;
	int	maxPt = _pts.nb();
	// add for points north, east, south and west
	// in order to 'avoid' infinite faces
	_pts.append(MPoint<double>(centerH, centerV - 2 * sizeV));
	_pts.append(MPoint<double>(centerH, centerV + 2 * sizeV));
	_pts.append(MPoint<double>(centerH - 2 * sizeH, centerV));
	_pts.append(MPoint<double>(centerH + 2 * sizeH, centerV));

	// setup the algorithm
	MArray<VoronoiAdjacency>	adjacencies;
	VoronoiEventByPos		evtCmp;
	MHeap<VoronoiEvent*>	events(&evtCmp);
	_first = NULL;
	_last = NULL;
	
	// prime the event heap with the input points
	for (int i=0;i<_pts.nb();i++) events.offer(new VoronoiEvent(_pts[i], i));
	
	// while there are events
	while ( events.nb() > 0 ) {
//		events.dump();
//		for (VoronoiArc* arc = _first;arc != NULL; arc = arc->_next) {
//			printf("%X -> ", arc);
//		}
//		printf("\n");
		VoronoiEvent*	evt = events.poll();
		if ( evt->_ends == NULL ) {
			// point insertion event: an arc is created
			// this arc may split a pre-existing arc, so we look for it
			// first
			refreshXs(evt->_pt._y);
			VoronoiArc*	inside = NULL;
			if ( _first == NULL ) {
				inside = NULL;
			} else if ( _first == _last ) {
				inside = _first;
			} else {
				for (VoronoiArc* arc = _first;arc != NULL; arc = arc->_next) {
					if ( arc->_next != NULL && evt->_pt._x <= arc->_right ) {
						inside = arc;
						break;
					} else if ( arc->_next == NULL ) {
						inside = arc;
					}
				}
			}
			if ( inside != NULL ) {
				VoronoiEvent*	old = inside->_end;
				inside->_end = NULL;
				if ( old != NULL ) {
					events.remove(old);
					//printf("del old %X\n", old);
					delete old;
				}
			}
			VoronoiArc*	nArc = new VoronoiArc(evt->_pt, evt->_ptNo);
			VoronoiArc* nRest = (inside==NULL) ? NULL : new VoronoiArc(inside->_pt, inside->_ptNo);
			if ( nArc != NULL ) {
				insert(nArc, inside, (inside==NULL)?NULL:inside->_next);
			}
			if ( nRest != NULL ) {
				insert(nRest, nArc, nArc->_next);				
			}
			if ( inside != NULL ) {
				VoronoiEvent*	nEnd = endOf(inside);
				if ( nEnd != NULL ) events.offer(nEnd);
			}
			if ( nArc != NULL ) {
				VoronoiEvent*	nEnd = endOf(nArc);
				if ( nEnd != NULL ) events.offer(nEnd);
			}
			if ( nRest != NULL ) {
				VoronoiEvent*	nEnd = endOf(nRest);
				if ( nEnd != NULL ) events.offer(nEnd);
			}
		} else {
			// arc disparition event
			VoronoiArc*	arc = evt->_ends;
			VoronoiArc*	prev = arc->_prev;
			VoronoiArc*	next = arc->_next;
			MPoint<double>	inter(evt->_pt._x, evt->_pt._y - evt->_radius);
			adjacencies.append(VoronoiAdjacency(arc->_ptNo, inter, arc->_pt));
			if ( prev != NULL ) {
				adjacencies.append(VoronoiAdjacency(prev->_ptNo, inter, prev->_pt));
			}
			if ( next != NULL ) {
				adjacencies.append(VoronoiAdjacency(next->_ptNo, inter, next->_pt));
			}
			if ( prev != NULL ) prev->_next = next;
			if ( next != NULL ) next->_prev = prev;
			if ( prev == NULL ) _first = next;
			if ( next == NULL ) _last = prev;
			if ( prev != NULL ) {
				VoronoiEvent*	old = prev->_end;
				prev->_end = NULL;
				if ( old != NULL ) {
					events.remove(old);
					//printf("del old %X\n", old);
					delete old;
				}
			}
			if ( next != NULL ) {
				VoronoiEvent*	old = next->_end;
				next->_end = NULL;
				if ( old != NULL ) {
					events.remove(old);
					//printf("del old %X\n", old);
					delete old;
				}
			}
			if ( prev != NULL ) {
				VoronoiEvent*	nEnd = endOf(prev);
				if ( nEnd != NULL ) events.offer(nEnd);
			}
			if ( next != NULL ) {
				VoronoiEvent*	nEnd = endOf(next);
				if ( nEnd != NULL ) events.offer(nEnd);
			}
			//printf("del arc %X\n", arc);
			delete arc;
		}
		//printf("del evt %X\n", evt);
		delete evt;
	}
	// clean up
	VoronoiArc* del = _first;
	while ( del != NULL ) {
		VoronoiArc* next = del->_next;
		delete del;
		del = next;
	}
	// build the graph representation
	if ( adjacencies.nb() > 0 ) {
		VoronoiAdjacencyByAng	adjCmp;
		adjacencies.sort(0, adjacencies.nb()-1, adjCmp);
		int		lastPtNo = -1;
		int		lastPtSt = -1;
		for (int i = 0; i < adjacencies.nb(); i++) {
			if ( adjacencies[i]._ptNo == lastPtNo ) {
				if ( adjacencies[i]._ptNo < maxPt ) res->lineto(adjacencies[i]._pt);
			} else {
				if ( lastPtSt >= 0 ) {
					if ( adjacencies[lastPtSt]._ptNo < maxPt ) {
						res->lineto(adjacencies[lastPtSt]._pt);
						res->close();
					}
				}
				lastPtNo = adjacencies[i]._ptNo;
				lastPtSt = i;
				if ( adjacencies[i]._ptNo < maxPt ) res->moveto(adjacencies[i]._pt);
			}
		}
	}
	return res;
}
void	Voronoi::refreshXs(double y) {
	double	x = 0;
	for (VoronoiArc* arc = _first; arc != NULL; arc = arc->_next) {
		if ( arc->_prev != NULL ) arc->_left = x;
		if ( arc->_next != NULL ) {
			double	xa = arc->_pt._x;
			double	ya = arc->_pt._y;
			double	pa = y - ya;
			double	xb = arc->_next->_pt._x;
			double	yb = arc->_next->_pt._y;
			double	pb = y - yb;
			double	a = 0.5 * ( 1 / pa - 1 / pb);
			double	b = -( xa / pa - xb / pb);
			double	c = 0.5 * (xa * xa + pa * pa) / pa - 0.5 * (xb * xb + pb * pb) / pb;
			double	discr = b * b - 4 * a * c;
			// there are always solutions to this computation, so discr < 0
			// is only a numerical imprecision
			if ( discr < 0 ) discr = 0;
			discr = sqrt(discr);
			if ( fabs(a) < 0.00001 ) {
				// here pa = pb, so the points are on the same horizontal line
				// the parabolic arcs intersection is at the middle
				x = 0.5 * ( xa + xb);
			} else {
				// if a is lower than b, then we need a point on its right
				// otherwise we need a point on the left of b
				double	r1 = (-b - discr ) / (2 * a);
				double	r2 = (-b + discr ) / (2 * a);
				if ( ya > yb + 0.0000001 ) {
					if ( r1 >= xa ) x = r1; else x = r2;
				} else if ( ya < yb - 0.0000001 ) { 
					if ( r2 <= xb ) x = r2; else x = r1;
				} else if ( xa < xb - 0.0000001 ) {
					x = 0.5 * ( xa + xb);
				} else if ( xa > xb + 0.0000001 ) {
					// this is most likely a point infinitely far away
					x = 100000000;
				} else {
					// the 2 arcs have their points roughly at the same position...
					// impossible to choose
					x=r2;
				}
			}
			arc->_right = x;
		}
	}
}

void	Voronoi::insert(VoronoiArc* arc, VoronoiArc* after, VoronoiArc* before) {
	arc->_prev = NULL;
	arc->_next = NULL;
	if ( _first == NULL ) {
		_first = _last = arc;
	} else {
		if ( after == NULL ) {
			arc->_next = _first;
			_first->_prev = arc;
			_first = arc;
		} else if ( after == _last ) {
			arc->_prev = after;
			after->_next = arc;
			_last = arc;
		} else {
			arc->_prev = after;
			after->_next = arc;
			arc->_next = before;
			before->_prev = arc;
		}
	}
}
VoronoiEvent*	Voronoi::endOf(VoronoiArc* arc) {
	arc->_end = NULL;
	if ( arc->_prev != NULL && arc->_next != NULL ) {
		double	xa = arc->_prev->_pt._x;
		double	ya = arc->_prev->_pt._y;
		double	xb = arc->_pt._x;
		double	yb = arc->_pt._y;
		double	xc = arc->_next->_pt._x;
		double	yc = arc->_next->_pt._y;
		if ( fabs(xa-xc) > 0.00001 || fabs(ya-yc) > 0.00001 ) {
			if ( (xa-xb) * (yc-yb) - (xc-xb) * (ya-yb) < 0 ) {
				double	a = xb - xa;
				double	b = yb - ya;
				double	c = xc - xa;
				double	d = yc - ya;
				double	e = 0.5 * (xb * xb - xa * xa + yb * yb - ya * ya);
				double	f = 0.5 * (xc * xc - xa * xa + yc * yc - ya * ya);
				double	det = a * d - b * c;
				if ( fabs(det) > 0.00001 ) {
					double	x = ( d * e - b * f ) / det;
					double	y = (-c * e + a * f ) / det;
					double	r = sqrt((xa-x)*(xa-x) + (ya-y)*(ya-y));
					arc->_end = new VoronoiEvent(MPoint<double>(x,y+r), arc, r);
					//printf("create evt %X for arc %X\n", arc->_end, arc);
				}
			}
		}
	}
	return arc->_end;
}

