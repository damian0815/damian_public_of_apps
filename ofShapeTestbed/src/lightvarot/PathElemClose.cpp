#include "PathElemClose.h"
#include "Polyline.h"
#include "Path.h"

PathElemClose::PathElemClose()  : PathElem() {
}
PathElemClose::PathElemClose(Path* dad) : PathElem(dad) {
}
PathElemClose::PathElemClose(PathElemClose const &o) : PathElem(o) {
}
PathElemClose::~PathElemClose() {
}

PathElem*	PathElemClose::clone(void) const {
	return new PathElemClose(_owner);
}

int			PathElemClose::type(void) const {
	return elem_close;
}
bool		PathElemClose::equals(PathElem const *o) const {
	if ( o->type() != elem_close ) return false;
	return true;
}
void		PathElemClose::dump(void) const {
	printf("Z \n");
}
PathElem*	PathElemClose::subPathElem(float st, float en) {
	return new PathElemClose(_owner);
}
void		PathElemClose::appendSubPathElem(Path* to, float st, float en, MPoint<float> exact) {
	to->close();
}
void		PathElemClose::convert(Polyline* to, float delta) {
	to->close(this);
}
void		PathElemClose::dirty(Region* to, float delta) {
}
bool			PathElemClose::hasEndTangent() {
	return false;
}
MPoint<float>	PathElemClose::endTangent() {
	return MPoint<float>(0,0);
}
bool			PathElemClose::hasStartTangent() {
	return false;
}
MPoint<float>	PathElemClose::startTangent() {
	return MPoint<float>(0,0);
}
void			PathElemClose::offset(Path* to, float offset, float delta, bool inSubpath, MPoint<float> const &firstT, MPoint<float> const &lastT, MPoint<float> const &lastP) {	
}
