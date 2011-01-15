#include "PathElemArc.h"
#include "Polyline.h"
#include "Path.h"
#include "Region.h"

PathElemArc::PathElemArc() :
	PathElem() {
}
PathElemArc::PathElemArc(MPoint<float> const &st, MPoint<float> const &center,
		MPoint<float> const &en, bool clockwise, Path* dad) :
	PathElem(dad), _st(st), _center(center), _en(en) {
	_clockwise = clockwise;
}
PathElemArc::PathElemArc(PathElemArc const &o) :
	PathElem(o), _st(o._st), _center(o._center), _en(o._en) {
	_clockwise = o._clockwise;
}
PathElemArc::~PathElemArc() {
}

PathElem* PathElemArc::clone(void) const {
	return new PathElemArc(_st, _center, _en, _clockwise, _owner);
}

int PathElemArc::type(void) const {
	return elem_arc;
}
bool PathElemArc::equals(PathElem const *o) const {
	if (o->type() != elem_arc)
		return false;
	PathElemArc* ol = (PathElemArc*) o;
	return _st == ol->_st && _center == ol->_center && _en == ol->_en;
}
void PathElemArc::dump(void) const {
	printf("A (%f %f) %f %f %f %f %i \n", _st._x, _st._y, _center._x, _center._y,
			_en._x, _en._y, ((_clockwise)?1:0));
}
bool PathElemArc::hasEnd() {
	return true;
}
MPoint<float> PathElemArc::endPoint() {
	return _en;
}
PathElem* PathElemArc::subPathElem(float st, float en) {
	MPoint<float> stD = _st - _center;
	MPoint<float> enD = _en - _center;
	double lst = sqrt(stD | stD);
	double len = sqrt(enD | enD);
	MPoint<double> stU = stD / lst;
	MPoint<double> stV(-stU._y, stU._x);
	MPoint<double> enU = enD / len;
	double co = stU | enU;
	double si = stV | enU;
	double an = (co <= -1 ) ? M_PI : (co >= 1) ? 0 : acos(co);
	if (si < 0)
		an = 2 * M_PI - an;
	if ( _clockwise == false ) an = an - 2 * M_PI;

	double stA = st * an;
	double enA = en * an;
	double stL = (1-st) * lst + (st) * len;
	double enL = (1-en) * lst + (en) * len;

	MPoint<double> nst = stL * (cos(stA) * stU + sin(stA) * stV);
	MPoint<double> nen = enL * (cos(enA) * stU + sin(enA) * stV);
	return new PathElemArc(MPoint<float>(nst._x, nst._y), _center, MPoint<float>(nen._x, nen._y), _clockwise, _owner);
}
void PathElemArc::appendSubPathElem(Path* to, float st, float en,
		MPoint<float> exact) {
	if ( st < en ) {
		to->arcto(_center, exact,_clockwise);
	} else {
		to->arcto(_center, exact,!_clockwise);
	}
}
void PathElemArc::convert(Polyline* to, float delta) {
	MPoint<float> stD = _st - _center;
	MPoint<float> enD = _en - _center;
	double lst = sqrt(stD | stD);
	double len = sqrt(enD | enD);
	MPoint<double> stU = stD / lst;
	MPoint<double> stV(-stU._y, stU._x);
	MPoint<double> enU = enD / len;
	double co = stU | enU;
	double si = stV | enU;
	double an = (co <= -1 ) ? M_PI : (co >= 1) ? 0 : acos(co);
	if (si < 0)
		an = 2 * M_PI - an;

	double ls = 0;
	if ( _clockwise ) {
		double a = 0;
		do {
			a+=0.1;
			if ( a > an ) {
				to->lineto(_en, ls, 1.0, this);
				break;
			}
			double s = a / an;
			double pL = (1-s) * lst + (s) * len;
			MPoint<double> p = pL * (cos(a) * stU + sin(a) * stV);
			to->lineto(_center + MPoint<float>(p._x, p._y), ls, s, this);
			ls=s;
		} while ( a < an);
	} else {
		an = an - 2 * M_PI;
		double a = 0;
		do {
			a-=0.1;
			if ( a < an ) {
				to->lineto(_en, ls, 1.0, this);
				break;
			}
			double s = a / an;
			double pL = (1-s) * lst + (s) * len;
			MPoint<double> p = pL * (cos(a) * stU + sin(a) * stV);
			to->lineto(_center + MPoint<float>(p._x, p._y), ls, s, this);
			ls=s;
		} while ( a > an);
	}
}
void PathElemArc::dirty(Region* to, float delta) {
	double	dang = 0.5;
	MPoint<float> stD = _st - _center;
	MPoint<float> enD = _en - _center;
	double lst = sqrt(stD | stD);
	double len = sqrt(enD | enD);
	MPoint<double> stU = stD / lst;
	MPoint<double> stV(-stU._y, stU._x);
	MPoint<double> enU = enD / len;
	double co = stU | enU;
	double si = stV | enU;
	double an = (co <= -1 ) ? M_PI : (co >= 1) ? 0 : acos(co);
	if (si < 0)
		an = 2 * M_PI - an;

	double maxR = 1 / cos(0.5 * dang);
	if ( _clockwise ) {
		double a = 0;
		double ma = 0;
		MPoint<float>	lPt = _st;
		do {
			ma = a + 0.5 * dang;
			a+=dang;
			if ( a > an ) a = an;
			if ( ma > an ) ma = an;
			double s = a / an;
			double pL = (1-s) * lst + (s) * len;
			MPoint<double> p = pL * (cos(a) * stU + sin(a) * stV);
			double ms = ma / an;
			double mpL = (1-ms) * lst + (ms) * len;
			MPoint<double> mp = maxR * mpL * (cos(ma) * stU + sin(ma) * stV);
			MPoint<float>	nPt(_center._x + p._x, _center._y + p._y); 
			MPoint<float>	nMid(_center._x + mp._x, _center._y + mp._y); 
			to->borderTriangle(lPt, nPt, nMid);
			to->insideEdge(lPt, nPt);
			lPt = nPt;
		} while ( a < an);
	} else {
		an = an - 2 * M_PI;
		double a = 0;
		double ma = 0;
		MPoint<float>	lPt = _st;
		do {
			ma = a - 0.5 * dang;
			a-=dang;
			if ( a < an ) a = an;
			if ( ma < an ) ma = an;
			double s = a / an;
			double pL = (1-s) * lst + (s) * len;
			MPoint<double> p = pL * (cos(a) * stU + sin(a) * stV);
			double ms = ma / an;
			double mpL = (1-ms) * lst + (ms) * len;
			MPoint<double> mp =maxR *  mpL * (cos(ma) * stU + sin(ma) * stV);
			MPoint<float>	nPt(_center._x + p._x, _center._y + p._y); 
			MPoint<float>	nMid(_center._x + mp._x, _center._y + mp._y); 
			to->borderTriangle(lPt, nPt, nMid);
			to->insideEdge(lPt, nPt);
			lPt = nPt;
		} while ( a > an);
	}
}
bool			PathElemArc::hasEndTangent() {
	MPoint<float>	dir = _center - _en;
	return (fabsf(dir._x) > 0.001 || fabsf(dir._y) > 0.001);
}
MPoint<float>	PathElemArc::endTangent() {
	MPoint<float>	dir = _center - _en;
	dir /= (float)sqrt(dir | dir);
	if ( _clockwise == true ) dir = -dir;
	return MPoint<float>(-dir._y, dir._x);
}
bool			PathElemArc::hasStartTangent() {
	MPoint<float>	dir = _center - _st;
	return (fabsf(dir._x) > 0.001 || fabsf(dir._y) > 0.001);
}
MPoint<float>	PathElemArc::startTangent() {
	MPoint<float>	dir = _center - _st;
	dir /= (float)sqrt(dir | dir);
	if ( _clockwise == true ) dir = -dir;
	return MPoint<float>(-dir._y, dir._x);
}
void			PathElemArc::offset(Path* to, float offs, float delta, bool inSubpath, MPoint<float> const &firstT, MPoint<float> const &lastT, MPoint<float> const &lastP) {	
	if (hasStartTangent() == false)
		return;
	MPoint<float>	sv = _center - _st;
	sv /= (float)sqrt(sv | sv);
	MPoint<float>	ev = _center - _en;
	ev /= (float)sqrt(ev | ev);
	if ( _clockwise == true ) {
		sv = -sv;
		ev = -ev;
	}
	MPoint<float>  su(-sv._y, sv._x);
	MPoint<float>  eu(-ev._y, ev._x);
	if (inSubpath == false) {
		to->moveto(_st - offs * sv);
	} else {
		double	ang = lastT ^ su;
		if ( ang * offs > -0.001 ) {
			to->lineto(_st - offs * sv);
		} else {
			to->arcto(_st, _st - offs * sv, (offs < 0) ? true : false);
		}
	}
	to->arcto(_center, _en - offs * ev, _clockwise);
}
