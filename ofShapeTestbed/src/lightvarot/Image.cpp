#include "Image.h"
#include "IPoint.h"
#include "MArray.h"

#include <png.h>

Image::Image(int sizeH, int sizeV) {
	_pts = new MHashset<IPoint>(new IPointHashByPos(), true);
	_sizeH = sizeH;
	_sizeV = sizeV;
	_sizeR = (_sizeH + 3) / 4; // make the row size a multiple of 4
	_sizeR *= 4;               // bad habit from the time page-aligned accesses where crucial
	_data = (uint8_t*) malloc(_sizeR * _sizeV * sizeof(uint8_t));
	_hasLastPt = false;
}

Image::~Image()
{
	delete _pts;
	free(_data);
	_data = NULL;
}

void Image::blank(uint8_t color) {
	for (int i=0; i<_sizeR * _sizeV; i++) {
		_data[i] = color;
	}
}
void Image::fill(int sx, int ex, int y, float c) {
	if ( c < -0.001 ) fill(sx,ex,y,-c);
	if (c > 0.001) {
		int ic = (int) (255 - 255 * c); // cover = 0 <=> white <=> pixel = 255
		// clamp pixel value to [0;255]
		if (ic < 0) ic = 0;
		if (ic > 255) ic = 255;
		// and clamp span extents to the line width
		if (sx < 0) sx = 0;
		if (ex >= _sizeH) ex = _sizeH - 1;
		// fill
		for (int x=sx; x<=ex; x++) {
			_data[y*_sizeR+x] = ic;
		}
	}
}

/*
 * textbook png writing (i like the smell of copy'n'paste in the morning)
 */
void Image::writeToFile(const char* fileName) {
	FILE *fp;
	png_structp png_ptr;
	png_infop info_ptr;

	/* open the file */
	fp = fopen(fileName, "wb");
	if (fp == NULL)
		return;

	/* Create and initialize the png_struct with the desired error handler
	 * functions.  If you want to use the default stderr and longjump method,
	 * you can supply NULL for the last three parameters.  We also check that
	 * the library version is compatible with the one used at compile time,
	 * in case we are using dynamically linked libraries.  REQUIRED.
	 */
	png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	if (png_ptr == NULL) {
		fclose(fp);
		return;
	}

	/* Allocate/initialize the image information data.  REQUIRED */
	info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == NULL) {
		fclose(fp);
		png_destroy_write_struct(&png_ptr, png_infopp_NULL);
		return;
	}

	/* Set error handling.  REQUIRED if you aren't supplying your own
	 * error handling functions in the png_create_write_struct() call.
	 */
	if (setjmp(png_jmpbuf(png_ptr))) {
		/* If we get here, we had a problem reading the file */
		fclose(fp);
		png_destroy_write_struct(&png_ptr, &info_ptr);
		return;
	}

	/* One of the following I/O initialization functions is REQUIRED */
	/* set up the output control if you are using standard C streams */
	png_init_io(png_ptr, fp);

	png_set_IHDR(png_ptr, info_ptr, _sizeH, _sizeV, 8, 
	PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, 
	PNG_FILTER_TYPE_DEFAULT);
	png_bytep row_pointers[_sizeV];
	for (int i=0; i<_sizeV; i++)
		row_pointers[i] = _data + (i*_sizeR);
	png_set_rows(png_ptr, info_ptr, row_pointers);
	/* This is the easy way.  Use it if you already have all the
	 * image info living info in the structure.  You could "|" many
	 * PNG_TRANSFORM flags into the png_transforms integer here.
	 */
	png_write_png(png_ptr, info_ptr, 0, png_voidp_NULL);

	/* clean up after the write, and free any memory allocated */
	png_destroy_write_struct(&png_ptr, &info_ptr);

	/* close the file */
	fclose(fp);
}

void    Image::blit(void) {
	// the hashset contains pixels with their coverage delta
	// basically, only the pixels on the eges
	// but hashset is not sorted so we sort the pixels by position
	// this array constructor doesn't allocate a new array
	MArray<IPoint>	pixs(_pts->data(), _pts->nb());
	IPointCmpByPos	ptCmp;
	pixs.sort(0, pixs.nb()-1, ptCmp);
	// run through all pixels, keeping a running sum (initialized to 0 at each new line)
	// of the coverage, and fill span of pixels with equal coverage
	int lastX = -1;
	int lastY = -1;
	float cover = 0;
	blank(0xff);
	for (int i=0;i<pixs.nb();i++) {
		int x = pixs[i]._x;
		int y = pixs[i]._y;
		float c = pixs[i]._val;
		if ( y == lastY ) {
			fill(lastX, x-1, lastY, cover);
		} else {
			if ( lastY >= 0 ) fill(lastX,_sizeH-1, lastY, cover);
			cover = 0;
		}
		cover += c;
		lastY = y;
		lastX = x;
	}
	if ( lastY >= 0 ) fill(lastX,_sizeH-1, lastY, cover);
}

void    Image::addDerivative(float sx, float sy, float ex, float ey, int minY, int maxY) {
	bool increment = ( ey > sy || ( fabsf(ey - sy) < 0.00001 && ex > sx ) );
	if ( fabsf(ex - sx) < 0.00001 && fabsf(ey - sy) < 0.00001 ) {
		// 0-length segment, so 0 contribution to coverage
	} else {
		// swap points to ensure (sx,sy) is the top endpoint
		if ( sy > ey ) {
			float x=sx;sx=ex;ex=x;
			float y=sy;sy=ey;ey=y;
		}
		// compute min/max line touched by this edge
		float minYF = floorf(sy);
		float maxYF = ceilf(ey);
		int minYI = (int) minYF;
		int maxYI = (int) maxYF;
		if ( minYI > maxY || maxYI < minY ) {
			// outside image
		} else {
			// clamp min/max line if needed
			if ( minYI < minY ) {
				minYI = minY;
				minYF = minY;
			}
			if ( maxYI > maxY ) {
				maxYI = maxY;
				maxYF = maxY;
			}
			// for each line, compute the intersection of the edge with the line
			// and create the deltas in coverage
			for (int curYI = minYI;curYI < maxYI;curYI++) {
				float stYF = (float) curYI;
				float enYF = stYF + 1.0f;
				if ( stYF < sy ) stYF = sy;
				if ( enYF > ey ) enYF = ey;
				if ( enYF > stYF + .0001 ) {
					float stXF = (sx * (ey - stYF) + ex * (stYF - sy)) / (ey - sy);
					float enXF = (sx * (ey - enYF) + ex * (enYF - sy)) / (ey - sy);
					addWedge( stXF, stYF, enXF, enYF, increment, curYI);
				} else {
					// horizontal -> no contribution
				}
			}
		}
	}
}
/*
 * add a single delta at pixel (x,y)
 */
void Image::addDelta(int x, int y, float delta) {
	if ( delta > 0.00001 || delta < -0.00001 ) {
		IPoint pt(x,y);
		float ndelta = delta;
		if ( _pts->contains(pt) ) { // adding delta => add old value to new one
			ndelta += (*_pts)[pt]._val;
		}
		pt._val = ndelta;
		_pts->put(pt);
	}
}

void Image::addWedge(float stXF, float stYF, float enXF, float enYF, bool increment,int curYI) {
	if ( stXF > enXF ) {
		float sw = stXF;stXF = enXF;enXF=sw;
	}
	float delta = (increment)?enYF - stYF:stYF - enYF;
	float length = enXF - stXF;
	float slope = (length > 0.00001) ? 1.0f / length : 0;
	float stXIF = floorf(stXF);
	float enXIF = ceilf(enXF);
	int stXI = (int) stXIF;
	int enXI = (int) enXIF;
	float curDelta = 0;
	float nDelta = 0;
	if ( stXI+1 == enXI ) {
		// 1 seul pixel
		nDelta = 0.5 * delta * (enXF - stXF) + delta * (enXIF - enXF);
		addDelta( stXI, curYI, nDelta);
	} else {
		nDelta = 0.5 * delta * (stXIF + 1.0f - stXF) * (stXIF + 1.0f - stXF) * slope;
		addDelta( stXI, curYI, nDelta-curDelta);
		curDelta = nDelta;
		for (int curXI = stXI+1;curXI < enXI-1;curXI++) {
			float curXIF = (float) curXI;
			nDelta = delta * 1.0f * (curXIF + 0.5f - stXF) * slope;
			addDelta( curXI, curYI, nDelta-curDelta);
			curDelta = nDelta;
		}
		nDelta = delta * 1.0f * (enXIF - 0.5f - stXF) * slope
		         - 0.5 * delta * (enXIF - enXF) * (enXIF - enXF) * slope;
		addDelta( enXI -1 , curYI, nDelta-curDelta);		
		curDelta = nDelta;
	}
	if ( fabsf(nDelta - delta) > 0.0001 ) {
		addDelta( enXI, curYI, delta-nDelta);
	}
}

/*
 * callbacks for polyline
 */
void	Image::moveto(MPoint<float> const &p, PathElem* origin) {
	_hasLastPt = true;
	_lastPt = p;
	_firstPt = p;
}
void	Image::lineto(MPoint<float> const &p, float stS, float enS, PathElem* origin) {
	if (  _hasLastPt ) {
		addDerivative(_lastPt._x, _lastPt._y , p._x, p._y, 0, _sizeV);
		_lastPt = p;
	}
}
void	Image::close(PathElem* origin) {
	if (  _hasLastPt ) {
		addDerivative(_lastPt._x, _lastPt._y , _firstPt._x, _firstPt._y, 0, _sizeV);
	}
	_hasLastPt = false;
}
