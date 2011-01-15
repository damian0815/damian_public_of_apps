/*
 * class representing a mask in 8-bits and implementing the edge-flag rasterization
 * 
 * the main purpose of this class is to be given edges of a polygon and rasterizing 
 * it. points on the edges are converted in coverage deltas, stored in a hashset,
 * then sorted and converted to pixel coverages. the hashset may be overkill when a
 * given pixel is hit by edges only a small number of times.
 * 
 * Image superclass Polyline to make it possible to raster a Path directly, but ideally
 * only path devoid of self-intersections should be used, since non intersection check is
 * done, and coverage is computed with the 'non-zero' winding rule
 */

#ifndef IMAGE_H_
#define IMAGE_H_

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "IPoint.h"
#include "MHashset.h"
#include "Polyline.h"

class Image : public Polyline {
public:
	int					_sizeH, _sizeV;	// dimensions
	int					_sizeR; // number of bytes in a row (multiple of 4)
	uint8_t*			_data; // pixels of the mask
	
private:
	MHashset<IPoint>	*_pts;	// pixels touched by edges
	bool				_hasLastPt; // datas for path conversion
	MPoint<float>		_lastPt;
	MPoint<float>		_firstPt;
	
public:
	Image(int sizeH, int sizeV);
	virtual ~Image();
	
	/*
	 * accessor
	 */
	MHashset<IPoint>*	getDeltas(void) {return _pts;};
	/*
	 * add an edge to the pixel deltas
	 */
	void	addDerivative(float sx, float sy, float ex, float ey, int minY, int maxY);
	/*
	 * adds a pixel in the pixel deltas
	 * used by the approximate coverage computations
	 */
	void	addDelta(int x, int y, float delta);

	/*
	 * converts the pixel deltas into pixel coverages
	 */
	void	blit(void);
	/*
	 * fill the pixels with a given coverage (0x00 = covered, 0xff = visible)
	 */
	void	blank(uint8_t color);
	/*
	 * output in png format
	 */
	void	writeToFile(const char* fileName);

private:
	/*
	 * utility for the edge-to-deltas conversion
	 * adds pixels for a infinite right-extending trapezoid
	 */
	void	addWedge(float stXF, float stYF, float enXF, float enYF, bool increment,int curYI);
	/*
	 * utility for filling the pixel coverages: fills a span of consecutive pixels
	 */
	void	fill(int sx, int ex, int y, float c);
	/*
	 * callbacks of the Polyline class for direct Path rasterization
	 */
	virtual void	moveto(MPoint<float> const &p, PathElem* origin);
	virtual void	lineto(MPoint<float> const &p, float stS, float enS, PathElem* origin);
	virtual	void	close(PathElem* origin);
};

#endif /*IMAGE_H_*/
