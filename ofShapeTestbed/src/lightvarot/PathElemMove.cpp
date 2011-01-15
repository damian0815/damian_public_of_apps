#include "PathElemMove.h"
#include "Polyline.h"
#include "Path.h"

PathElemMove::PathElemMove()  : PathElem(), _pt(){
}
PathElemMove::PathElemMove(MPoint<float> const &pt, Path* dad) : PathElem(dad), _pt(pt) {
}
PathElemMove::PathElemMove(PathElemMove const &o) : PathElem(o), _pt(o._pt) {
}
PathElemMove::~PathElemMove() {
}

PathElem*	PathElemMove::clone(void) const {
	return new PathElemMove(_pt, _owner);
}

int			PathElemMove::type(void) const {
	return elem_move;
}
bool		PathElemMove::equals(PathElem const *o) const {
	if ( o->type() != elem_move ) return false;
	return _pt == ((PathElemMove*)(o))->_pt;
}
void		PathElemMove::dump(void) const {
	printf("M %f %f \n", _pt._x, _pt._y);
}
bool			PathElemMove::hasEnd() {
	return true;
}
MPoint<float>	PathElemMove::endPoint() {
	return _pt;
}
PathElem*	PathElemMove::subPathElem(float st, float en) {
	return new PathElemMove(_pt, _owner);
}
void		PathElemMove::appendSubPathElem(Path* to, float st, float en, MPoint<float> exact) {
	to->moveto(exact);
}
void		PathElemMove::convert(Polyline* to, float delta) {
	to->moveto(_pt, this);
}
void		PathElemMove::dirty(Region* to, float delta) {
}
bool			PathElemMove::hasEndTangent() {
	return false;
}
MPoint<float>	PathElemMove::endTangent() {
	return MPoint<float>(0,0);
}
bool			PathElemMove::hasStartTangent() {
	return false;
}
MPoint<float>	PathElemMove::startTangent() {
	return MPoint<float>(0,0);
}
void			PathElemMove::offset(Path* to, float offset, float delta, bool inSubpath, MPoint<float> const &firstT, MPoint<float> const &lastT, MPoint<float> const &lastP) {	
}
