#include "VarotPolygon.h"
#include "Path.h"
#include "PathElem.h"


ReformWalker::ReformWalker(MArray<WindingEdge> const &ed, MArray<WindingAdjacency> const &ad
						, MArray<FPoint> const &pt) : Walker(ed, ad)
						                                                            , nshape()
		                      														, _pts(pt+0, pt.nb()) {
	nshape = new Path();
}
ReformWalker::~ReformWalker() {
	if ( nshape != NULL ) delete nshape;
}

/*
 * beginning of a loop in the tour
 * 
 * since the point is found by taking the topmost not-yet visited point, the starting
 * adjacency may lie in the middle of a curve patch. this function moves the starting
 * point to the begin of the curve patch, or the closest crossing in the polygon,
 * whichever happens first
 */
int		ReformWalker::newConnectedComponent(int startAdj) {
	int startPt = _adjs[startAdj]._pt;
	{
		int nAdj = startAdj;
		while ( _edges[_adjs[nAdj]._edge]._stA != nAdj || _edges[_adjs[nAdj]._edge]._visited == true) {
			nAdj++;
			if ( nAdj >= _adjs.nb() || _adjs[nAdj]._pt != startPt ) {
				nAdj--;
				while ( nAdj >= 0 && _adjs[nAdj]._pt == startPt ) nAdj--;
				nAdj++;
			}
			if ( nAdj == startAdj ) break; // tant pis
		}
		startAdj = nAdj;
	}
	_curSrc = _edges[_adjs[startAdj]._edge]._orig;
	_startS =  _adjs[startAdj]._S;
	do {
		startPt = _adjs[startAdj]._pt;
		if ( _pts[startPt]._no != 2 ) break; 
		int nAdj = startAdj;
		do {
			nAdj++;
			if ( nAdj >= _adjs.nb() || _adjs[nAdj]._pt != startPt ) {
				nAdj--;
				while ( nAdj >= 0 && _adjs[nAdj]._pt == startPt ) nAdj--;
				nAdj++;
			}
			if ( _edges[_adjs[nAdj]._edge]._visited == false )  break;
		} while ( nAdj != startAdj );
		if ( nAdj == startAdj ) break;
		int	ned = _adjs[nAdj]._edge;
		if ( _edges[ned]._orig != _curSrc ) break;
		if ( fabsf(_adjs[nAdj]._S - _startS) > 0.00001 ) break;
		startAdj = _edges[ned].other(nAdj);
		startPt = _adjs[startAdj]._pt;
		_startS = _adjs[startAdj]._S;
	} while ( true );
	_endS = _startS;
	nshape->moveto(_pts[startPt]);
	_inSubpath = true;
	return startAdj;
}
/*
 * move clockwise on the polygon
 */
int     ReformWalker::circleAtPoint(int nAdj, int ptNo, bool clockwise) {
	return Walker::circleAtPoint(nAdj, ptNo, clockwise);
}
/*
 * end of a loop in the path -> close the current subpath
 */
void	ReformWalker::preBacktrack(int curAdj, int wNo,  bool clockwise, WindingEdge* curWind) {
	if ( _inSubpath ) {
		_inSubpath = false;
		_curSrc->appendSubPathElem(nshape, _startS, _endS, _pts[_adjs[curAdj]._pt]);
		nshape->close();
	}
}
/*
 * cross a point, or begin a loop
 * 
 * if a loop begin, make the moveto
 * if a loop continues, test whether the curve origin changes, or whether the point 
 *   is a crossing or whether the abcissis is non-continuous: in that case create a curve patch
 */
void	ReformWalker::advanceAtPoint(int curAdj, int wNo,  bool clockwise, WindingEdge* curWind) {
	if ( _inSubpath ) {
		int	ned = _adjs[curAdj]._edge;
		if ( _edges[ned]._orig != _curSrc 
				|| fabsf(_adjs[curAdj]._S - _endS) > 0.00001 
				|| _pts[_adjs[curAdj]._pt]._no != 2 ) {
			_curSrc->appendSubPathElem(nshape, _startS, _endS, _pts[_adjs[curAdj]._pt]);
			_startS = _endS = _adjs[curAdj]._S;
			_curSrc = _edges[ned]._orig;
		}
	} else {
		nshape->moveto(_pts[_adjs[curAdj]._pt]);
		int	ned = _adjs[curAdj]._edge;
		_startS = _adjs[curAdj]._S;
		_curSrc = _edges[ned]._orig;
		_inSubpath = true;
	}
}
void	ReformWalker::postAdvanceAtEdge(int curAdj, int wNo,  bool clockwise, WindingEdge* curWind) {
	_endS = _adjs[curAdj]._S;

}
Path*	ReformWalker::givePath(void) {
	Path* res = nshape;
	nshape = NULL;
	return res;
}
