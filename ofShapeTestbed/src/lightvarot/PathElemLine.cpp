#include "PathElemLine.h"
#include "Polyline.h"
#include "Region.h"
#include "Path.h"

PathElemLine::PathElemLine() : PathElem() {
}
PathElemLine::PathElemLine(MPoint<float> const &st, MPoint<float> const &en, Path* dad) 
             : PathElem(dad), _st(st), _en(en) {
}
PathElemLine::PathElemLine(PathElemLine const &o)
    : PathElem(o), _st(o._st), _en(o._en) {	
}
PathElemLine::~PathElemLine() {	
}

PathElem*	PathElemLine::clone(void) const {
	return new PathElemLine(_st,_en,_owner);
}

int			PathElemLine::type(void) const {
	return elem_line;
}
bool		PathElemLine::equals(PathElem const *o) const {
	if ( o->type() != elem_line ) return false;
	PathElemLine*	ol = (PathElemLine*) o;
	return _st == ol->_st && _en == ol->_en;
}
void		PathElemLine::dump(void) const {
	printf("L (%f %f) %f %f \n", _st._x, _st._y, _en._x, _en._y);
}
bool			PathElemLine::hasEnd() {
	return true;
}
MPoint<float>	PathElemLine::endPoint() {
	return _en;
}
PathElem*	PathElemLine::subPathElem(float st, float en) {
	MPoint<float>	stP = (1-st) * _st + (st) * _en;
	MPoint<float>	enP = (1-en) * _st + (en) * _en;
	return new PathElemLine(stP, enP, _owner);
}
void		PathElemLine::appendSubPathElem(Path* to, float st, float en, MPoint<float> exact) {
	to->lineto(exact);
}
void		PathElemLine::convert(Polyline* to, float delta) {
	to->lineto(_en, 0,1, this);
}
void		PathElemLine::dirty(Region* to, float delta) {
	to->insideEdge(_st, _en);
	to->borderLine(_st, _en);
}
bool			PathElemLine::hasEndTangent() {
	MPoint<float>	dir = _en - _st;
	return (fabsf(dir._x) > 0.001 || fabsf(dir._y) > 0.001);
}
MPoint<float>	PathElemLine::endTangent() {
	MPoint<float>	dir = _en - _st;
	dir /= (float)sqrt(dir | dir);
	return dir;
}
bool			PathElemLine::hasStartTangent() {
	MPoint<float>	dir = _en - _st;
	return (fabsf(dir._x) > 0.001 || fabsf(dir._y) > 0.001);
}
MPoint<float>	PathElemLine::startTangent() {
	MPoint<float>	dir = _en - _st;
	dir /= (float)sqrt(dir | dir);
	return dir;
}
void			PathElemLine::offset(Path* to, float offs, float delta, bool inSubpath, MPoint<float> const &firstT, MPoint<float> const &lastT, MPoint<float> const &lastP) {	
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
	to->lineto(_en + offs * v);
}
