#include "Polyline.h"

Polyline::Polyline()
{
	_firstIndex = -1;
	_lastIndex = -1;
}

Polyline::~Polyline()
{
}
void	Polyline::moveto(MPoint<float> const &p, PathElem* origin) {
	FPoint	rp = p;
	rp.round();
	_lastPt = p;
	_lastRoundedPt = rp;
	_lastIndex = createPoint(_lastRoundedPt);
	_firstPt = _lastPt;
	_firstRoundedPt = _lastRoundedPt;
	_firstIndex = _lastIndex;
}
void	Polyline::lineto(MPoint<float> const &p, float stS, float enS, PathElem* origin) {
	if ( _lastIndex >= 0 ) {
		FPoint	rp = p;
		rp.round();
		if ( rp == _lastRoundedPt ) {
		} else {
			_lastPt = p;
			_lastRoundedPt = rp;
			int nIndex = createPoint(_lastRoundedPt);
			createEdge(_lastIndex, nIndex, stS, enS, origin);
			_lastIndex = nIndex;
		}
	}
}
void	Polyline::close(PathElem* origin) {
	if ( _lastIndex >= 0 && _firstIndex >= 0 ) {
		createEdge(_lastIndex, _firstIndex, 0, 1, origin);
	}
}

int		Polyline::createPoint(FPoint const &p) {
	return -1;
}
int		Polyline::createEdge(int st, int en, float stS, float enS, PathElem* origin) {
	return -1;
}
