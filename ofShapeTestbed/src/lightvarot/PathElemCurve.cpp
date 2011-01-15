#include "PathElemCurve.h"
#include "Polyline.h"
#include "Region.h"
#include "Path.h"

PathElemCurve::PathElemCurve() :
	PathElem() {
}
PathElemCurve::PathElemCurve(MPoint<float> const &st, MPoint<float> const &p1,
		MPoint<float> const &p2, MPoint<float> const &en, Path* dad) :
	PathElem(dad), _st(st), _p1(p1), _p2(p2), _en(en) {
}
PathElemCurve::PathElemCurve(PathElemCurve const &o) :
	PathElem(o), _st(o._st), _p1(o._p1), _p2(o._p2), _en(o._en) {
}
PathElemCurve::~PathElemCurve() {
}

PathElem* PathElemCurve::clone(void) const {
	return new PathElemCurve(_st, _p1, _p2, _en, _owner);
}

int PathElemCurve::type(void) const {
	return elem_curve;
}
bool PathElemCurve::equals(PathElem const *o) const {
	if (o->type() != elem_curve)
		return false;
	PathElemCurve* ol = (PathElemCurve*) o;
	return _st == ol->_st && _p1 == ol->_p1 && _p2 == ol->_p2 && _en == ol->_en;
}
void PathElemCurve::dump(void) const {
	printf("C (%f %f) %f %f %f %f %f %f \n", _st._x, _st._y, _p1._x, _p1._y,
			_p2._x, _p2._y, _en._x, _en._y);
}
bool PathElemCurve::hasEnd() {
	return true;
}
MPoint<float> PathElemCurve::endPoint() {
	return _en;
}
PathElemCurve* PathElemCurve::subPathStart(float s) {
	MPoint<float> p01 = (1-s) * _st + (s) * _p1;
	MPoint<float> p12 = (1-s) * _p1 + (s) * _p2;
	MPoint<float> p23 = (1-s) * _p2 + (s) * _en;
	MPoint<float> p012 = (1-s) * p01 + (s) * p12;
	MPoint<float> p123 = (1-s) * p12 + (s) * p23;
	MPoint<float> p0123 = (1-s) * p012 + (s) * p123;
	return new PathElemCurve(_st, p01, p012, p0123, _owner);
}
PathElemCurve* PathElemCurve::subPathEnd(float s) {
	MPoint<float> p01 = (1-s) * _st + (s) * _p1;
	MPoint<float> p12 = (1-s) * _p1 + (s) * _p2;
	MPoint<float> p23 = (1-s) * _p2 + (s) * _en;
	MPoint<float> p012 = (1-s) * p01 + (s) * p12;
	MPoint<float> p123 = (1-s) * p12 + (s) * p23;
	MPoint<float> p0123 = (1-s) * p012 + (s) * p123;
	return new PathElemCurve(p0123, p123, p23, _en, _owner);
}
PathElem* PathElemCurve::subPathElem(float st, float en) {
	if (st < en) {
		PathElemCurve* tmp = subPathEnd(st);
		PathElemCurve* res = tmp->subPathStart((en-st) / (1-st));
		delete tmp;
		return res;
	} else {
		PathElemCurve* tmp = subPathStart(st);
		PathElemCurve* rev = tmp->subPathEnd(en / st);
		PathElemCurve* res = new PathElemCurve(rev->_en, rev->_p2, rev->_p1, rev->_st, _owner);
		delete tmp;
		delete rev;
		return res;
	}
}
void PathElemCurve::appendSubPathElem(Path* to, float st, float en,
		MPoint<float> exact) {
	if (st < en) {
		PathElemCurve* tmp = subPathEnd(st);
		PathElemCurve* res = tmp->subPathStart((en-st) / (1-st));
		to->curveto(res->_p1, res->_p2, exact);
		delete res;
		delete tmp;
	} else {
		PathElemCurve* tmp = subPathStart(st);
		PathElemCurve* res = tmp->subPathEnd(en / st);
		to->curveto(res->_p2, res->_p1, exact);
		delete res;
		delete tmp;
	}
}
void PathElemCurve::convert(Polyline* to, float delta) {
	convertCurve(to, delta, _st, _p1, _p2, _en, 0, 1);
}
void PathElemCurve::convertCurve(Polyline* to, float delta,
		MPoint<float> const &p0, MPoint<float> const &p1, MPoint<float> const &p2,
		MPoint<float> const &p3, float p0S, float p3S) {

	MPoint<float> d03 = p3 - p0;
	MPoint<float> d01 = p1 - p0;
	MPoint<float> d12 = p2 - p1;
	MPoint<float> d23 = p3 - p2;
	double d03l = sqrt(d03|d03);
	double d01l = sqrt(d01|d01);
	double d12l = sqrt(d12|d12);
	double d23l = sqrt(d23|d23);
	double ecart = d01l + d12l + d23l - d03l;
	if (ecart < delta) {
		to->lineto(p3, p0S, p3S, this);
	} else {
		float mid = 0.5 * (p0S + p3S);
		MPoint<float> p01 = (0.5f) * p0 + (0.5f) * p1;
		MPoint<float> p12 = (0.5f) * p1 + (0.5f) * p2;
		MPoint<float> p23 = (0.5f) * p2 + (0.5f) * p3;
		MPoint<float> p012 = (0.5f) * p01 + (0.5f) * p12;
		MPoint<float> p123 = (0.5f) * p12 + (0.5f) * p23;
		MPoint<float> p0123 = (0.5f) * p012 + (0.5f) * p123;
		convertCurve(to, delta, p0, p01, p012, p0123, p0S, mid);
		convertCurve(to, delta, p0123, p123, p23, p3, mid, p3S);
	}
}
void PathElemCurve::dirty(Region* to, float delta) {
	dirtyCurve(to, delta, _st, _p1, _p2, _en, 0, 1);
}
void PathElemCurve::dirtyCurve(Region* to, float delta, MPoint<float> const &p0,
		MPoint<float> const &p1, MPoint<float> const &p2, MPoint<float> const &p3,
		float p0S, float p3S) {

	MPoint<float> d03 = p3 - p0;
	MPoint<float> d01 = p1 - p0;
	MPoint<float> d12 = p2 - p1;
	MPoint<float> d23 = p3 - p2;
	double d03l = sqrt(d03|d03);
	double d01l = sqrt(d01|d01);
	double d12l = sqrt(d12|d12);
	double d23l = sqrt(d23|d23);
	double ecart = d01l + d12l + d23l - d03l;
	if (ecart < delta) {
		to->insideEdge(p0, p3);
		to->borderQuad(p0, p1, p2, p3);
	} else {
		float mid = 0.5 * (p0S + p3S);
		MPoint<float> p01 = (0.5f) * p0 + (0.5f) * p1;
		MPoint<float> p12 = (0.5f) * p1 + (0.5f) * p2;
		MPoint<float> p23 = (0.5f) * p2 + (0.5f) * p3;
		MPoint<float> p012 = (0.5f) * p01 + (0.5f) * p12;
		MPoint<float> p123 = (0.5f) * p12 + (0.5f) * p23;
		MPoint<float> p0123 = (0.5f) * p012 + (0.5f) * p123;
		dirtyCurve(to, delta, p0, p01, p012, p0123, p0S, mid);
		dirtyCurve(to, delta, p0123, p123, p23, p3, mid, p3S);
	}
}

bool PathElemCurve::hasEndTangent() {
	MPoint<float> dir = _en - _p2;
	if (fabsf(dir._x)> 0.001 || fabsf(dir._y)> 0.001)
		return true;
	dir = _en - _p1;
	if (fabsf(dir._x)> 0.001 || fabsf(dir._y)> 0.001)
		return true;
	dir = _en - _st;
	return (fabsf(dir._x)> 0.001 || fabsf(dir._y)> 0.001);
}
MPoint<float> PathElemCurve::endTangent() {
	MPoint<float> dir = _en - _p2;
	if (fabsf(dir._x)> 0.001 || fabsf(dir._y)> 0.001) {
		dir /= (float) sqrt(dir | dir);
		return dir;
	}
	dir = _en - _p1;
	if (fabsf(dir._x)> 0.001 || fabsf(dir._y)> 0.001) {
		dir /= (float) sqrt(dir | dir);
		return dir;
	}
	dir = _en - _st;
	if (fabsf(dir._x)> 0.001 || fabsf(dir._y)> 0.001) {
		dir /= (float) sqrt(dir | dir);
		return dir;
	}
	return MPoint<float>(0, 0);
}
bool PathElemCurve::hasStartTangent() {
	MPoint<float> dir = _p1 - _st;
	if (fabsf(dir._x)> 0.001 || fabsf(dir._y)> 0.001)
		return true;
	dir = _p2 - _st;
	if (fabsf(dir._x)> 0.001 || fabsf(dir._y)> 0.001)
		return true;
	dir = _en - _st;
	return (fabsf(dir._x)> 0.001 || fabsf(dir._y)> 0.001);
}
MPoint<float> PathElemCurve::startTangent() {
	MPoint<float> dir = _p1 - _st;
	if (fabsf(dir._x)> 0.001 || fabsf(dir._y)> 0.001) {
		dir /= (float) sqrt(dir | dir);
		return dir;
	}
	dir = _p2 - _st;
	if (fabsf(dir._x)> 0.001 || fabsf(dir._y)> 0.001) {
		dir /= (float) sqrt(dir | dir);
		return dir;
	}
	dir = _en - _st;
	if (fabsf(dir._x)> 0.001 || fabsf(dir._y)> 0.001) {
		dir /= (float) sqrt(dir | dir);
		return dir;
	}
	return MPoint<float>(0, 0);
}
void PathElemCurve::offset(Path* to, float offs, float delta, bool inSubpath,
		MPoint<float> const &firstT, MPoint<float> const &lastT,
		MPoint<float> const &lastP) {
	if (hasStartTangent() == false)
		return;
	MPoint<float> u = startTangent();
	u /= (float)sqrt(u|u);
	MPoint<float> v(-u._y, u._x);
	if (inSubpath == false) {
		to->moveto(_st + offs * v);
	} else {
		double	ang = lastT ^ u;
		if ( ang * offs > -0.001 ) {
			to->lineto(_st + offs * v);
		} else {
			to->arcto(_st, _st + offs * v, (offs < 0) ? true : false);
		}
	}
	offset(to, offs, delta, 0,1);
}
void	PathElemCurve::offset(Path* to, float offs, float delta, float stS, float enS) {
	MPoint<float> p01 = (1-stS) * _st + (stS) * _p1;
	MPoint<float> p12 = (1-stS) * _p1 + (stS) * _p2;
	MPoint<float> p23 = (1-stS) * _p2 + (stS) * _en;
	MPoint<float> p012 = (1-stS) * p01 + (stS) * p12;
	MPoint<float> p123 = (1-stS) * p12 + (stS) * p23;
	MPoint<float> p0123 = (1-stS) * p012 + (stS) * p123;
	float nEnS = (enS-stS) / (1-stS);
	MPoint<float> q01 = (1-nEnS) * p0123 + (nEnS) * p123;
	MPoint<float> q12 = (1-nEnS) * p123 + (nEnS) * p23;
	MPoint<float> q23 = (1-nEnS) * p23 + (nEnS) * _en;
	MPoint<float> q012 = (1-nEnS) * q01 + (nEnS) * q12;
	MPoint<float> q123 = (1-nEnS) * q12 + (nEnS) * q23;
	MPoint<float> q0123 = (1-nEnS) * q012 + (nEnS) * q123;
	
	MPoint<float>	stU = p123 - p012;
	MPoint<float>	enU = q123 - q012;
	MPoint<float>	k0 = _p2 + _st -  _p1 * 2.0f;
	MPoint<float>	k1 = _en + _p1 -  _p2 * 2.0f;
	MPoint<float>	stK = (1-stS) * k0 + (stS) * k1;
	MPoint<float>	enK = (1-enS) * k0 + (enS) * k1;
	
	double	stUL = sqrt(stU | stU);
	double	enUL = sqrt(enU | enU);
	stU /= (float) stUL;
	enU /= (float) enUL;
	
	double	stR = (stK ^ stU) / (stUL * stUL);
	double	enR = (enK ^ enU) * (1-stS) * (1-stS) / (enUL * enUL);

	stR = (fabs(stR) < 0.00001) ? 100000 : 1 / stR;
	enR = (fabs(enR) < 0.00001) ? 100000 : 1 / enR;
	float	stM = (stR+offs) / stR;
	float	enM = (enR+offs) / enR;
	
	MPoint<float>	stV(-stU._y, stU._x);
	MPoint<float>	enV(-enU._y, enU._x);
	MPoint<float>	t0 = p0123 + offs * stV;
	MPoint<float>	t1 = p0123 + (q01-p0123) * stM + offs * stV;
	MPoint<float>	t2 = q0123 + (q012-q0123) * enM + offs * enV;
	MPoint<float>	t3 = q0123 + offs * enV;

	float midS = 0.5;
	MPoint<float> r01 = (1-midS) * p0123 + (midS) * q01;
	MPoint<float> r12 = (1-midS) * q01 + (midS) * q012;
	MPoint<float> r23 = (1-midS) * q012 + (midS) * q0123;
	MPoint<float> r012 = (1-midS) * r01 + (midS) * r12;
	MPoint<float> r123 = (1-midS) * r12 + (midS) * r23;
	MPoint<float> r0123 = (1-midS) * r012 + (midS) * r123;

	MPoint<float> s01 = (1-midS) * t0 + (midS) * t1;
	MPoint<float> s12 = (1-midS) * t1 + (midS) * t2;
	MPoint<float> s23 = (1-midS) * t2 + (midS) * t3;
	MPoint<float> s012 = (1-midS) * s01 + (midS) * s12;
	MPoint<float> s123 = (1-midS) * s12 + (midS) * s23;
	MPoint<float> s0123 = (1-midS) * s012 + (midS) * s123;

	MPoint<float>	miU = r123 - r012;
	MPoint<float>	caU = s123 - s012;
	miU /= (float) sqrt(miU | miU);
	caU /= (float) sqrt(caU | caU);
	
	MPoint<float>	miV(-miU._y, miU._x);
	MPoint<float>	dP = r0123 + offs * miV - s0123;
	MPoint<float>	dT = caU - miU;
	
	double	err = (dP | dP) + (dT | dT); 
	
	if ( err < 0.001 || enS - stS < 0.001 ) {
		to->curveto(t1, t2, t3);
	} else {
		float m = 0.5 * (stS + enS);
		offset(to, offs, delta, stS, m);
		offset(to, offs, delta, m, enS);
	}
}

