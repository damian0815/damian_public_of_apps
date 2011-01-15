#include "Region.h"
#include "MArray.h"

Region::Region(int sizeH, int sizeV, float scale)
{
	_sizeH = sizeH;
	_sizeV = sizeV;
	_scale = scale;
	
	float	ssH = ceilf(((float)_sizeH) / scale);
	float	ssV = ceilf(((float)_sizeV) / scale);
	
	_sSizeH = (int)ssH;
	_sSizeV = (int)ssV;
	
	_inside = new Image(_sSizeH, _sSizeV);
	_border = new Image(_sSizeH, _sSizeV);
	
	for (int i=0;i<=_sSizeV;i++) {
		_convexLeft.append(0);
		_convexRight.append(0);
	}
}

Region::~Region()
{
	if ( _inside != NULL ) delete _inside;
	if ( _border != NULL ) delete _border;
	_inside = NULL;
	_border = NULL;
}
void		Region::finish(void) {
	mergeInside();
}
void		Region::mergeInside(void) {
	MHashset<IPoint>* insidePts = _inside->getDeltas();

	MArray<IPoint>	pixs(insidePts->data(), insidePts->nb());
	IPointCmpByPos	ptCmp;
	pixs.sort(0, pixs.nb()-1, ptCmp);

	int lastY = -1;
	float cover = 0;
	int	ish = _sSizeH;
	for (int i=0;i<pixs.nb();i++) {
		int x = pixs[i]._x;
		int y = pixs[i]._y;
		float c = pixs[i]._val;
		//fill(x, x+1, y, c);
		if ( y == lastY ) {
			if ( fabsf(cover) < 0.001 && fabsf(c + cover) >= 0.001 ) {
				_border->addDelta(x,y,1);
			} else if ( fabsf(cover) >= 0.001 && fabsf(c+cover) < 0.001 ) {
				_border->addDelta(x,y,-1);
			}
		} else {
			if ( fabsf(cover) >= 0.001 && lastY >= 0 ) _border->addDelta(ish, lastY, -1);
			cover = 0;
			if ( fabsf(c) >= 0.001 ) _border->addDelta(x, y, 1);
		}
		cover += c;
		lastY = y;
		//lastX = x;
	}
	if ( fabsf(cover) >= 0.001 && lastY >= 0 ) _border->addDelta(ish, lastY, -1);
}

void		Region::insideEdge(MPoint<float> const &a, MPoint<float> const &b) {
	_inside->addDerivative(a._x / _scale, a._y/ _scale, b._x/ _scale, b._y/ _scale, 0, _sSizeV);
}
void		Region::borderLine(MPoint<float> const &a, MPoint<float> const &b) {
	convexInit();
	convexAdd(a / _scale, b / _scale);
	convexFlush();
}
void		Region::borderTriangle(MPoint<float> const &a, MPoint<float> const &b, MPoint<float> const &c) {
	convexInit();
	convexAdd(a / _scale, b / _scale);
	convexAdd(b / _scale, c / _scale);
	convexAdd(c / _scale, a / _scale);
	convexFlush();
}
void		Region::borderQuad(MPoint<float> const &a, MPoint<float> const &b, MPoint<float> const &c, MPoint<float> const &d) {
	convexInit();
	// add  all edges to the leftmost/rightmost array, so that we are sure the four 'border'
	// edges have been added
	convexAdd(a / _scale, b / _scale);
	convexAdd(b / _scale, c / _scale);
	convexAdd(c / _scale, d / _scale);
	convexAdd(d / _scale, a / _scale);
	convexAdd(a / _scale, c / _scale);
	convexAdd(b / _scale, d / _scale);
	convexFlush();
}
void		Region::convexInit(void) {
	for (int i=0;i<_convexLeft.nb();i++) {
		_convexLeft[i] = _sSizeH + 1;
		_convexRight[i] = -1;
	}
}
void		Region::convexAdd(MPoint<float> const &a, MPoint<float> const &b) {
	float	sx = a._x;
	float	sy = a._y;
	float	ex = b._x;
	float	ey = b._y;
	if ( sy > ey ) {
		float sw;
		sw = sx; sx = ex; ex = sw;
		sw = sy; sy = ey; ey = sw;
	}
	float	minYF = floorf(sy);
	float	maxYF = ceilf(ey);
	int		minYI = (int) minYF;
	int		maxYI = (int) maxYF;
	if ( maxYI < 0 || minYI >= _sSizeV ) return;
	if ( maxYI >= _sSizeV ) maxYI = _sSizeV-1;
	if ( minYI < 0 ) minYI = 0;
	if ( minYI == maxYI ) {
		// sur une ligne entre 2 pixels
		// mais il faut qd meme le prendre en compte
		convexSpanAdd(minYI, sx, ex);
	} else {
		for (int curYI = minYI;curYI < maxYI;curYI++) {
			float stYF = (float) curYI;
			float enYF = stYF + 1.0f;
			if ( stYF < sy ) stYF = sy;
			if ( enYF > ey ) enYF = ey;
			if ( enYF > stYF + .0001 ) {
				float stXF = (sx * (ey - stYF) + ex * (stYF - sy)) / (ey - sy);
				float enXF = (sx * (ey - enYF) + ex * (enYF - sy)) / (ey - sy);
				convexSpanAdd(curYI, stXF, enXF);
			} else {
				// horizontal 
				convexSpanAdd(curYI, sx, ex);
			}
		}		
	}
}
void		Region::convexSpanAdd(int y, float a, float b) {
	if ( y < 0 || y > _sSizeV ) return;
	if ( a > b ) {
		float s = a; a = b; b = s;
	}
	int	minX = (int)floorf(a);
	int	maxX = (int)ceilf(b);
	if ( minX < _convexLeft[y] ) _convexLeft[y] = minX;
	if ( maxX > _convexRight[y] ) _convexRight[y] = maxX;
}
// flush to the set of pixel deltas describing the dirty region
void		Region::convexFlush(void) {
	for (int i=0;i<_sSizeV;i++) {
		int 	min = _convexLeft[i];
		int 	max = _convexRight[i];
		if ( min <= max ) {
			_border->addDelta(min, i, 1);
			_border->addDelta(max+1, i, -1);
		}
	}
}
