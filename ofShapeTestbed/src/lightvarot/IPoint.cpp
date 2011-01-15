#include "IPoint.h"

IPoint::IPoint(void) : MPoint<int>() {
	_hash = 0;
	_val = 0;
	_tmp = -1;
}
IPoint::IPoint(const IPoint &o) : MPoint<int>(o) {
	_hash = o._hash;
	_val = o._val;
	_tmp = -1;
}
IPoint::IPoint(int x, int y) : MPoint<int>(x,y) {
	// very dumb hash function
	_hash = (x * 11658165) + y;
	if ( _hash < 0 ) _hash = -_hash;
	_val = 0;
	_tmp = -1;
}
IPoint::~IPoint(void) {
	
}
int			IPoint::getNext(void) {
	return _tmp;
}
void		IPoint::setNext(int to) {
	_tmp=to;
}
void		IPoint::dump(void) {
}
