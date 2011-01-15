#include "FPoint.h"

FPoint::FPoint(void) : MPoint<float>() {
}
FPoint::FPoint(const FPoint &o) : MPoint<float>(o) {
	_no = o._no;
}
FPoint::FPoint(const MPoint<float> &o) : MPoint<float>(o) {
}
FPoint::FPoint(const MPoint<double> &o) : MPoint<float>(o._x, o._y) {
}
FPoint::FPoint(float x, float y) : MPoint<float>(x,y) {
}
FPoint::~FPoint(void) {
}
bool		FPoint::equals(const FPoint &o) const {
	return _x == o._x && _y == o._y;
}
void		FPoint::dump(void) {
}
int			FPoint::getNext(void) {
	return _tmp;
}
void		FPoint::setNext(int to) {
	_tmp=to;
}

void		FPoint::round(void) {
	// ldexpf() supposedly only touches the mantissa
	// so it  * should *  be fast
	_x = ldexpf(roundf(ldexpf(_x, rounding_power)), -rounding_power);
	_y = ldexpf(roundf(ldexpf(_y, rounding_power)), -rounding_power);
}


