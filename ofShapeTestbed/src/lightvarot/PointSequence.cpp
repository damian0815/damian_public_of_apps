#include "PointSequence.h"
#include "Path.h"

#define N03(t) ((1.0-t)*(1.0-t)*(1.0-t))
#define N13(t) (3*(t)*(1.0-t)*(1.0-t))
#define N23(t) (3*(t)*(t)*(1.0-t))
#define N33(t) ((t)*(t)*(t))

PointSequence::PointSequence() : _pts() {
}
PointSequence::~PointSequence() {
}
void PointSequence::addPoint(MPoint<float> const &p) {
	_pts.append(FitPoint(MPoint<double>(p._x, p._y)));
}

Path* PointSequence::simplify(void) {
	// limit cases:
	// - 0 point -> no path
	// - 1 point -> just a moveto
	// - 2 point -> a single line
	Path* res = new Path();
	if (_pts.nb() <= 0)
		return res;
	res->moveto(MPoint<float>(_pts[0]._pt._x, _pts[0]._pt._y));
	if (_pts.nb() == 1)
		return res;
	if (_pts.nb() == 2) {
		res->lineto(MPoint<float>(_pts[1]._pt._x, _pts[1]._pt._y));
		return res;
	}
	// at least 3 points, so we can make a curve
	int stP = 0, enP = _pts.nb() - 1;

	// first fit
	initPositions(stP, enP);
	calcControlPoints(stP, enP);

	// improve a bit
	nudgePositions(stP, enP);
	calcControlPoints(stP, enP);

	// again...
	nudgePositions(stP, enP);
	calcControlPoints(stP, enP);

	// again...
	nudgePositions(stP, enP);
	calcControlPoints(stP, enP);

	// should be enough
	refreshDelta(stP, enP);

	// make the cubic bezier patch
	res->curveto(MPoint<float>(_P1._x, _P1._y), MPoint<float>(_P2._x, _P2._y),
			MPoint<float>(_P3._x, _P3._y));

	return res;
}

/*
 * storage class for the dynamic programming
 */
class SimplifyPoint {
public:
	int _prev;
	double _delta;
	SimplifyPoint() {
		_prev = -1;
		_delta =0;
	};
	SimplifyPoint(int prev, double del) {
		_prev = prev;
		_delta =del;
	};
	SimplifyPoint(SimplifyPoint const &o) {
		_prev = o._prev;
		_delta =o._delta;
	};
	virtual ~SimplifyPoint(void) {
	};
};
/*
 * convert to the complete simplify()
 */
Path* PointSequence::simplify(double delta, bool approx) {
	return simplify(-1, delta, approx);
}
Path* PointSequence::simplify(int nbElem, bool approx) {
	return simplify(nbElem, -1, approx);
}
/*
 * dynamic programming applied to curve fitting:
 * - a matrix of nbPoint * nbPatch (where nbPatch is the max number of patches in
 *   the result curve) where each entry (n,p) has the minimum of average distance to
 *   curve for a curve approximating n points with p patches, and a back pointer
 *   to the entry before it that gave this minimum
 * - if approx = false, an entry (n,p) is computed 'exactly' by trying to extend all
 *   previous entries with p-1 patches and less than n points
 * - if approx = true, an entry (n,p) is computed by extending in an exact fashion
 *   entries [n-1;n-20], and then in an approximate fashion the entries before:
 *   instead of fitting a curve for each number of of points before n, we fit curves
 *   every 10 points (and every 100 points before n-200 points) and interpolate the
 *   average distance to the curve inbetween
 *
 * if approx = true, then the dynamic programming is skewed a bit: instead of
 * computing the increment in
 */
Path* PointSequence::simplify(int nbElem, double delta, bool approx) {
	Path* res = new Path();
	if (_pts.nb() <= 0)
		return res;
	res->moveto(MPoint<float>(_pts[0]._pt._x, _pts[0]._pt._y));
	if (_pts.nb() == 1)
		return res;
	if (_pts.nb() == 2) {
		res->lineto(MPoint<float>(_pts[1]._pt._x, _pts[1]._pt._y));
		return res;
	}
	// at least 3 points -> cuuuuurve
	int maxPatch = (_pts.nb() + 2) / 3;
	if ( nbElem > 0 ) maxPatch = nbElem+1;
	MArray<SimplifyPoint> best(_pts.nb() * maxPatch, true);
	int lastP = _pts.nb() - 1;
	for (int enP = 1; enP <=lastP; enP++) {
		if (approx) {
			int stP = enP-1;
			int dStP = 1;
			double lastDelta = 0;
			int lastStP = enP;
			while (stP >= 0) {
				initPositions(stP, enP);
				calcControlPoints(stP, enP);

				nudgePositions(stP, enP);
				calcControlPoints(stP, enP);

				refreshDelta(stP, enP);

				for (int rStP = stP; rStP < lastStP; rStP++) {
					// interpolate the delta
					// when enP -20 <= stP < enP, then lastStP = stP + 1 so
					// exDelta end up being _delta
					double exDelta = (_delta * ((double)(lastStP - rStP))
							+ lastDelta * ((double)(rStP - stP)))
							/ ((double)(lastStP-stP));
					for (int n = 0; n<maxPatch-1; n++) {
						int stIdx = n * _pts.nb() + stP;
						int enIdx = (n+1) * _pts.nb() + enP;
						if (best[stIdx]._prev >= 0 || (stP == 0 && n == 0)) {
							double nDelta = best[stIdx]._delta + exDelta;
							if ( best[enIdx]._prev < 0 || best[enIdx]._delta> nDelta) {
								best[enIdx] = SimplifyPoint(stP, nDelta);
							}
						}
					}
				}
				lastDelta = _delta;
				lastStP = stP;
				if (enP - stP > 20)
					dStP = 10;
				if (enP - stP > 200)
					dStP = 100;
				stP -= dStP;
				if (stP < 0 && stP > -dStP)
					stP=0; // if stP was already 0, then it's < 0 after here, and
				           // the while() loop exits
			}
		} else {
			for (int stP=0; stP<enP; stP++) {
				initPositions(stP, enP);
				calcControlPoints(stP, enP);

				nudgePositions(stP, enP);
				calcControlPoints(stP, enP);

				refreshDelta(stP, enP);

				for (int n = 0; n<maxPatch-1; n++) {
					int stIdx = n * _pts.nb() + stP;
					int enIdx = (n+1) * _pts.nb() + enP;
					if (best[stIdx]._prev >= 0 || (stP == 0 && n == 0)) {
						double nDelta = best[stIdx]._delta + _delta;
						if ( best[enIdx]._prev < 0 || best[enIdx]._delta> nDelta) {
							best[enIdx] = SimplifyPoint(stP, nDelta);
						}
					}
				}
			}
		}
	}

//	for (int i=0; i<lastP; i++) {
//		printf("%i: ", i);
//		for (int n=0; n<maxPatch; n++) {
//			int idx = n * _pts.nb() + i;
//			printf("(%i %f) ", best[idx]._prev, best[idx]._delta);
//		}
//		printf("\n");
//	}

	double maxDelta = delta * ((double) lastP);
	int minNb = -1;
	double minDelta = -1;
	// find optimum
	if ( nbElem > 0 ) {
		for (int n=0; n<=nbElem; n++) {
			int idx = n * _pts.nb() + lastP;
			if (best[idx]._prev >= 0) {
				if (minNb < 0 || best[idx]._delta < minDelta) {
					minDelta = best[idx]._delta;
					minNb = n;
				}
			}
		}
	} else if ( delta > 0 ) {
		for (int n=0; n<maxPatch; n++) {
			int idx = n * _pts.nb() + lastP;
			if (best[idx]._prev >= 0) {
				if (best[idx]._delta < maxDelta) {
					if (minNb < 0 || n < minNb) {
						minNb = n;
					}
				}
			}
		}
	} else {
		// no target: neither a number of cubic patches, nor a max delta
		// from the curve -> do nothing...
	}

	if (minNb >= 0) {
		// backtrack in the matrix
		MArray<int> poss;
		int curP = lastP;
		poss.append(curP);
		for (int curN=minNb; curN >= 0; curN--) {
			int idx = curN * _pts.nb() + curP;
			int prev = best[idx]._prev;
			if (prev >= 0) {
				poss.append(prev);
			}
			curP = prev;
		}
//		printf("poss: ");
//		for (int i=0; i<poss.nb(); i++) {
//			printf("(%i %i) ", i, poss[i]);
//		}
//		printf("\n");
		// refit points to curves, because we didn't store the results
		// we fitted so many curves at this point, one or two more won't
		// make a big differnce
		// plus in the approximation mode, we didn't compute all curves
		for (int i=poss.nb()-1; i>0; i--) {
			int enP = poss[i-1];
			int stP = poss[i];
			initPositions(stP, enP);
			calcControlPoints(stP, enP);

			nudgePositions(stP, enP);
			calcControlPoints(stP, enP);

			res->curveto(MPoint<float>(_P1._x, _P1._y), MPoint<float>(_P2._x,
					_P2._y), MPoint<float>(_P3._x, _P3._y));
		}
	}
	return res;
}
/*
 * init abcissis with a seed value
 * we use a chordal approximation here
 */
void PointSequence::initPositions(int stP, int enP) {
	_pts[stP]._l = 0;
	_pts[stP]._t = 0;
	MPoint<double> lastP = _pts[stP]._pt;
	double lastT = _pts[stP]._t;
	for (int i=stP + 1; i<=enP; i++) {
		MPoint<double> dir = _pts[i]._pt - lastP;
		double nl = sqrt(dir|dir);
		_pts[i]._l = nl;
		_pts[i]._t = lastT + nl;
		lastP = _pts[i]._pt;
		lastT += nl;
	}
	double totL = lastT;
	for (int i=stP; i<enP; i++)
		_pts[i]._t /= totL;

}
/*
 * one step of newtow-rhapson to 'improve' abcisses
 *
 * the goal is to get the point on the curve with minimum distance to the target
 * point, so we are looking for a root of the derivative of the distance
 * (square of the distance actually, because it's easier to compute)
 */
void PointSequence::nudgePositions(int stP, int enP) {
	_delta = 0;
	for (int i=stP+1; i<enP; i++) {
		double t = _pts[i]._t;
		// compute the current point on the curve
		MPoint<double> p01 = (1-t) * _P0 + t * _P1;
		MPoint<double> p12 = (1-t) * _P1 + t * _P2;
		MPoint<double> p23 = (1-t) * _P2 + t * _P3;
		MPoint<double> p012 = (1-t) * p01 + t * p12;
		MPoint<double> p123 = (1-t) * p12 + t * p23;
		MPoint<double> p0123 = (1-t) * p012 + t * p123;
		// compute the derivative of the current point on the curve
		MPoint<double> d01 = (1-t) * (_P1 - _P0) + t * (_P2 - _P1);
		MPoint<double> d12 = (1-t) * (_P2 - _P1) + t * (_P3 - _P2);
		MPoint<double> d012 = (1-t) * d01 + t * d12;
		// compute the second derivative of the current point on the curve
		MPoint<double> dd01 = (1-t) * (_P2 - (2.0 * _P1) + _P0) + t * (_P3 - (2.0 * _P2) + _P1);

		MPoint<double> mp = p0123 - _pts[i]._pt;
		MPoint<double> dm = d012;
		MPoint<double> ddm = dd01;

		// distance
		_delta += mp|mp;
		// derivative of the squared distance
		double f = 2 * (mp | dm);
		// second derivative of the squared distance
		double df = 2 * (mp | ddm) + 2 * (dm | dm);
		// newton-rhapson
		double dt = (fabs(df) < 0.00001) ? 0 : 0.5 * f / df;
		// clamp the new abcissis to the previous and next abcissis
		double nt = _pts[i]._t - dt;
		if (nt < _pts[i-1]._t)
			nt = _pts[i-1]._t;
		if (nt > _pts[i+1]._t)
			nt = _pts[i+1]._t;
		if (nt < 0)
			nt = 0;
		if (nt > 1)
			nt = 1;
		_pts[i]._t = nt;
	}
}
/*
 * juste recompute the average distance of points to curve, like in
 * nudgePositions()
 */
void PointSequence::refreshDelta(int stP, int enP) {
	_delta = 0;
	for (int i=stP+1; i<enP; i++) {
		double t = _pts[i]._t;
		MPoint<double> p01 = (1-t) * _P0 + t * _P1;
		MPoint<double> p12 = (1-t) * _P1 + t * _P2;
		MPoint<double> p23 = (1-t) * _P2 + t * _P3;
		MPoint<double> p012 = (1-t) * p01 + t * p12;
		MPoint<double> p123 = (1-t) * p12 + t * p23;
		MPoint<double> p0123 = (1-t) * p012 + t * p123;

		MPoint<double> mp = p0123 - _pts[i]._pt;

		_delta += mp|mp;
	}
}
/*
 * fit a bezier cubic through points [stP;enP]
 *
 * the method is a simple minimization of the squared distance to curve.
 * the trick is that this distance is not the 'exact' distance, which would be
 * the distance between a point and its projection on the curve, but the distance
 * between a point and its estimatation. so the distance is conditionned by the
 * set of abcisses.
 * in the end the global fitting process is:
 * 1) get a seed set of abcisses
 * 2) find the curve minimizing the distances from the points to their approximation
 *   with the abcissis on the curve
 * 3) move abcisses a bit to make them closer to the projection of the point on the
 *   estimated curve
 * 4) repeat 2) and 3) until satisfied
 *
 * there is no convergence guarantee, but all this is used for 'visual' approximation
 * so there is no real need to be exact when dealing with something of inherently
 * subjective nature
 *
 * the computations are pretty standard linear algebra
 * for more explanations, see:
 * http://www.cs.mtu.edu/~shene/COURSES/cs3621/NOTES/INT-APP/CURVE-APP-global.html
 */
void PointSequence::calcControlPoints(int stP, int enP) {
	_P0 = _pts[stP]._pt;
	_P3 = _pts[enP]._pt;
	_P1 = _P0;
	_P2 = _P3;
	for (int i=stP+1; i<enP; i++) {
		_pts[i]._q = _pts[i]._pt - N03(_pts[i]._t) * _P0 - N33(_pts[i]._t) * _P3;
	}
	MPoint<double> Q1(0, 0);
	MPoint<double> Q2(0, 0);
	double m00 = 0, m01 = 0, m10 = 0, m11 = 0;
	for (int i=stP+1; i<enP; i++) {
		double n1= N13(_pts[i]._t);
		double n2= N23(_pts[i]._t);
		Q1 += n1 * _pts[i]._q;
		Q2 += n2 * _pts[i]._q;
		m00 += n1 * n1;
		m01 += n1 * n2;
		m10 += n2 * n1;
		m11 += n2 * n2;
	}
	double det = m00*m11 - m01*m10;
	if (fabs(det) > 0.00001) {
		double r00 = m11 / det;
		double r01 = -m01 / det;
		double r10 = -m10 / det;
		double r11 = m00 / det;

		_P1 = r00 * Q1 + r01 * Q2;
		_P2 = r10 * Q1 + r11 * Q2;
	}
}

void PointSequence::toFile(const char* fileName) {
	FILE *fp = fopen(fileName, "wb");
	if (fp == NULL)
		return;

	fprintf(fp, "<?xml version=\"1.0\"?>\n");
	fprintf(
			fp,
			"<!DOCTYPE svg PUBLIC \"-//W3C//DTD SVG 1.0//EN\" \"http://www.w3.org/TR/2001/REC-SVG-20010904/DTD/svg10.dtd\">\n");
	fprintf(fp, "<svg xmlns=\"http://www.w3.org/2000/svg\">\n");

	/*
	 * a circle + text for each point
	 */
	fprintf(fp, "<g style=\"fill-opacity:0.7;\">\n");
	for (int i=0; i<_pts.nb(); i++) {
		fprintf(
				fp,
				"  <circle cx=\"%fmm\" cy=\"%fmm\" r=\"1mm\" style=\"fill:red; stroke:black; stroke-width:0.2mm\" />\n",
				_pts[i]._pt._x, _pts[i]._pt._y);

		double t = _pts[i]._t;
		MPoint<double> p01 = (1-t) * _P0 + t * _P1;
		MPoint<double> p12 = (1-t) * _P1 + t * _P2;
		MPoint<double> p23 = (1-t) * _P2 + t * _P3;
		MPoint<double> p012 = (1-t) * p01 + t * p12;
		MPoint<double> p123 = (1-t) * p12 + t * p23;
		MPoint<double> p0123 = (1-t) * p012 + t * p123;

		fprintf(
				fp,
				"  <circle cx=\"%fmm\" cy=\"%fmm\" r=\"1mm\" style=\"fill:green; stroke:black; stroke-width:0.2mm\" />\n",
				p0123._x, p0123._y);
	}
	fprintf(fp, "</g>\n");

	/*
	 * a line + text for each edge
	 */
	fprintf(fp, "<g style=\" fill-opacity:1;\">\n");
	for (int i=1; i<_pts.nb(); i++) {
		float sx = _pts[i-1]._pt._x;
		float sy = _pts[i-1]._pt._y;
		float ex = _pts[i]._pt._x;
		float ey = _pts[i]._pt._y;
		fprintf(
				fp,
				"  <line x1=\"%fmm\" y1=\"%fmm\" x2=\"%fmm\" y2=\"%fmm\" style=\"fill:none; stroke:black; stroke-width:0.2mm\"/>\n",
				sx, sy, ex, ey);
	}

	fprintf(fp, "</g>\n");

	fprintf(fp, "</svg>\n");

	fclose(fp);
}
