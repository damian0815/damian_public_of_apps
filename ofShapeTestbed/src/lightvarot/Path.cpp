#include "Path.h"
#include "PathElem.h"

#include "PathElemMove.h"
#include "PathElemLine.h"
#include "PathElemClose.h"
#include "PathElemCurve.h"
#include "PathElemArc.h"

#include "Region.h"

Path::Path() : _elems() {
	_firstPtSet = false;
	_lastPtSet = false;
}
Path::Path(Path const &o) : _elems(o._elems.nb()){
	for (int i=0;i<o._elems.nb();i++) {
		_elems.append(o._elems[i]->clone(this));
	}
	_firstPtSet = false;
	_lastPtSet = false;
}
Path::~Path() {
	for (int i=0;i<_elems.nb();i++) delete _elems[i];
}

void		Path::moveto(MPoint<float> pt) {
	PathElem*	n = new PathElemMove(pt, this);
	_elems.append(n);
	_firstPtSet = true;
	_firstPt = pt;
	_lastPtSet = true;
	_lastPt = pt;
}
void		Path::lineto(MPoint<float> pt) {
	if ( _lastPtSet == false ) return;
	PathElem*	n = new PathElemLine(_lastPt, pt, this);
	_elems.append(n);
	_lastPtSet = true;
	_lastPt = pt;
}
void		Path::curveto(MPoint<float> p1, MPoint<float> p2, MPoint<float> pt) {
	if ( _lastPtSet == false ) return;
	PathElem*	n = new PathElemCurve(_lastPt, p1, p2, pt, this);
	_elems.append(n);
	_lastPtSet = true;
	_lastPt = pt;
}
void		Path::arcto(MPoint<float> center, MPoint<float> pt, bool clockwise) {
	if ( _lastPtSet == false ) return;
	PathElem*	n = new PathElemArc(_lastPt, center, pt, clockwise, this);
	_elems.append(n);
	_lastPtSet = true;
	_lastPt = pt;
}
void		Path::close(void) {
	if ( _lastPtSet == false || _firstPtSet == false ) return;
	if ( _firstPt == _lastPt ) {
		// already on the subpath start point -> no need for an additional edge
	} else {
		lineto(_firstPt);
	}
	PathElem*	n = new PathElemClose(this);
	_elems.append(n);
	_lastPtSet = false;
	_firstPtSet = false;
}

void		Path::fill(Polyline* to, float delta) {
	for (int  i = 0; i<_elems.nb();i++) _elems[i]->convert(to, delta);
}
void		Path::dirty(Region* to, float delta) {
	for (int  i = 0; i<_elems.nb();i++) _elems[i]->dirty(to, delta);
	to->finish();
}

void		Path::dump(void) {
	printf("path %i :\n", (int)this);
	for (int i=0;i<_elems.nb();i++) _elems[i]->dump();
}
Path*		Path::offset(float offset, float delta) {
	Path*	res = new Path();
	bool	inSubpath = false;
	MPoint<float>	firstT, lastT, lastP;
	for (int i=0;i<_elems.nb();i++) {
		_elems[i]->offset(res, offset, delta, inSubpath, firstT, lastT, lastP);
		if ( _elems[i]->hasEndTangent() ) {
			lastT = _elems[i]->endTangent();
			lastP = _elems[i]->endPoint();
		}
		if ( inSubpath == false ) {
			if ( _elems[i]->hasStartTangent() ) {
				firstT = _elems[i]->startTangent();
				inSubpath = true;
			}
		}
		if ( _elems[i]->hasEnd() == false ) {
			if ( inSubpath ) {
				MPoint<float>	v(-firstT._y, firstT._x);
				if ( (lastT^firstT) * offset > -0.001) {
					res->lineto(lastP + offset * v);
				} else {
					res->arcto(lastP , lastP + offset * v, (offset < 0) ? true : false);
				}
			}
			inSubpath = false;
		}
	}
	return res;
}

