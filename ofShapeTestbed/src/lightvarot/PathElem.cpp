#include "PathElem.h"
#include "Path.h"
#include "Polyline.h"


PathElem::PathElem() {
	_owner = NULL;
}
PathElem::PathElem(Path* dad) {
	_owner = dad;
}
PathElem::PathElem(PathElem const &o) {
	_owner = o._owner;
}
PathElem::~PathElem() {
	_owner = NULL;
}
int			PathElem::type(void) const {
	return elem_none;
}
PathElem*	PathElem::clone(void) const {
	return new PathElem();
}
PathElem*	PathElem::clone(Path* dad) const {
	PathElem*	res = clone();
	res->_owner = dad;
	return res;
}


bool	PathElem::equals(PathElem const *o) const {
	return ( o->type() == elem_none );
}
void	PathElem::dump(void) const {
	
}

bool			PathElem::hasEnd() {
	return false;
}
MPoint<float>	PathElem::endPoint() {
	return MPoint<float>(0,0);
}
PathElem*	PathElem::subPathElem(float st, float en) {
	return new PathElem(_owner);
}
void		PathElem::appendSubPathElem(Path* to, float st, float en, MPoint<float> exact) {
}
void		PathElem::convert(Polyline* to, float delta) {
}
void		PathElem::dirty(Region* to, float delta) {
}

bool		PathElem::hasEndTangent() {
	return false;
}
MPoint<float>	PathElem::endTangent() {
	return MPoint<float>(0,0);
}
bool		PathElem::hasStartTangent() {
	return false;
}
MPoint<float>	PathElem::startTangent() {
	return MPoint<float>(0,0);
}
void	PathElem::offset(Path* to, float offset, float delta, bool inSubpath, MPoint<float> const &firstT, MPoint<float> const &lastT, MPoint<float> const &lastP) {	
}

