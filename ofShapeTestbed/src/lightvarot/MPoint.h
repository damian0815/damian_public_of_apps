/*
 * template for a 2D point, with coordinates stored in the given format
 * 
 * implements the basic algebra functions: +, -, * and / by scalars
 */

#ifndef my_tpoint
#define my_tpoint

#include <math.h>

template <typename T> class MPoint {
public:
	T		_x, _y;
	
	MPoint(void) {};
	MPoint(T const x, T const y) {_x = x; _y = y;};
	MPoint(MPoint<T> const &o) {_x = o._x;_y = o._y;};
	virtual ~MPoint(void) {};
	
	MPoint<T>& operator=(MPoint<T> const &b) {
		_x = b._x;
		_y = b._y;
		return *this;
	};
	MPoint<T>& operator+=(MPoint<T> const &b) {
		_x += b._x;
		_y += b._y;
		return *this;
	};
	MPoint<T>& operator-=(MPoint<T> const &b) {
		_x -= b._x;
		_y -= b._y;
		return *this;
	};
	MPoint<T>& operator*=(T b) {
		_x *= b;
		_y *= b;
		return *this;
	};
	MPoint<T>& operator/=(T b) {
		_x /= b;
		_y /= b;
		return *this;
	};
};

template <class T> bool operator==(MPoint<T> const &a, MPoint<T> const &b) {
	return (a._x == b._x && a._y == b._y);
};
template <class T> MPoint<T> operator+(MPoint<T> const &a, MPoint<T> const &b) {
	return MPoint<T>(a._x + b._x, a._y + b._y);
};
template <class T> MPoint<T> operator-(MPoint<T> const &a, MPoint<T> const &b) {
	return MPoint<T>(a._x - b._x, a._y - b._y);
};
template <class T> MPoint<T> operator-( MPoint<T> const &b) {
	return MPoint<T>(- b._x, - b._y);
};
template <class T> MPoint<T> operator*(MPoint<T> const &a, float b) {
	return MPoint<T>(a._x * b, a._y * b);
};
template <class T> MPoint<T> operator/(MPoint<T> const &a, float b) {
	return MPoint<T>(a._x / b, a._y / b);
};
template <class T> MPoint<T> operator*(float b, MPoint<T> const &a) {
	return MPoint<T>(a._x * b, a._y * b);
};


// in case the multiplying/dividing scalar is a double, we force computations in
// doubles to retain full precision
template <class T> MPoint<double> operator*(MPoint<T> const &a, double b) {
	return MPoint<double>(((double)a._x) * b, ((double)a._y) * b);
};
template <class T> MPoint<double> operator*(double b, MPoint<T> const &a) {
	return MPoint<double>(((double)a._x) * b, ((double)a._y) * b);
};
template <class T> MPoint<double> operator/(MPoint<T> const &a, double b) {
	return MPoint<double>(((double)a._x) / b, ((double)a._y) / b);
};

// special case: product of float coordinates make double values
// this is supposed to be enough for intersection computations
double operator|(MPoint<float> const &a, MPoint<float> const &b);
double operator^(MPoint<float> const &a, MPoint<float> const &b);
double operator|(MPoint<double> const &a, MPoint<double> const &b);
double operator^(MPoint<double> const &a, MPoint<double> const &b);

#endif
