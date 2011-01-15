#include "MPoint.h"

double operator|(MPoint<float> const &a, MPoint<float> const &b) {
	return ((double)a._x) * ((double)b._x) + ((double)a._y) * ((double)b._y);
}
double operator^(MPoint<float> const &a, MPoint<float> const &b) {
	return ((double)a._x) * ((double)b._y) - ((double)a._y) * ((double)b._x);
}
double operator|(MPoint<double> const &a, MPoint<double> const &b) {
	return ((double)a._x) * ((double)b._x) + ((double)a._y) * ((double)b._y);
}
double operator^(MPoint<double> const &a, MPoint<double> const &b) {
	return ((double)a._x) * ((double)b._y) - ((double)a._y) * ((double)b._x);
}
