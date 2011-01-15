#include "VarotPolygon.h"

WindingWalker::WindingWalker(MArray<WindingEdge> const &ed, MArray<WindingAdjacency> const &ad
		                   , MArray<IntersEdge> const &in, MArray<FPoint> const &pt) : Walker(ed,ad)
		                                                                             , _inters(in+0, in.nb())
		                                                                             , _pts(pt+0, pt.nb()) {
	_curNum0 = 0;
	_curNum1 = 0;
}
WindingWalker::~WindingWalker() {
	
}
/*
 * start a connected component in the polygon. it inits the winding numbers for the
 * groups 'A' and 'B' by checking the horizontal line passing through the start point
 * against edges
 * 
 * this function is called for each hole and island in the polygon
 * a single call to this function is harmless, but calling it for each hole 
 * can suck up a lot of computation time
 */
int		WindingWalker::newConnectedComponent(int startAdj) {
	int		startPt = _adjs[startAdj]._pt;
	FPoint	start = _pts[startPt];
	_curNum0 = 0;
	_curNum1 = 0;
	for (int i=0;i<_inters.nb();i++) {
		if ( _inters[i]._top < start._y && _inters[i]._bottom >= start._y ) {
			double	sx = _inters[i]._st._x, sy = _inters[i]._st._y;
			double	ex = _inters[i]._en._x, ey = _inters[i]._en._y;
			double	px = start._x, py = start._y;
			double ix = (sx * (ey - py) + ex * (py - sy)) / (ey - sy);
			if ( ix < px ) {
				if ( _inters[i]._group == 0 ) {
					_curNum0 += (_inters[i]._downward) ? -1 : 1;
				} else if ( _inters[i]._group == 1 ) {
					_curNum1 += (_inters[i]._downward) ? -1 : 1;
				}
			}
		}
	}
	return startAdj;
}
int     WindingWalker::circleAtPoint(int nAdj, int ptNo, bool clockwise) {
	_curNum0 = _edges[_adjs[nAdj]._edge].getWinding0(nAdj, !clockwise );
	_curNum1 = _edges[_adjs[nAdj]._edge].getWinding1(nAdj, !clockwise );
	return Walker::circleAtPoint(nAdj, ptNo, clockwise);
}
void	WindingWalker::postBacktrack(int curAdj, int wNo,  bool clockwise, WindingEdge* curWind) {
	_curNum0 = _edges[wNo].getWinding0(curAdj, !clockwise);
	_curNum1 = _edges[wNo].getWinding1(curAdj, !clockwise);
}
void	WindingWalker::preAdvanceAtEdge(int curAdj, int wNo,  bool clockwise, WindingEdge* curWind) {
	_edges[wNo].setWinding(curAdj, clockwise, _curNum0, _curNum1, curWind);
}
	

