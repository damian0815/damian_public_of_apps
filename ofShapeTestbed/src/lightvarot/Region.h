/*
 * a dirty region for a path
 * 
 * the region is meant to be stored in an array of pixels (ideally a bitmap for fast intersection
 * test and storage size), where each pixel correspond to a square on the canvas. here the
 * squares have size _scale and the canvas is a rectangle of sizes _sizeH and _sizeV
 * 
 * the dirty squares are obtained in 2 ways:
 * - the path is coarsely converted, and the 'interior' of this coarse approximation (with
 *   non-zero winding rule) is taken
 * - an estimation of the deviation of the actual border to the coarse approximation is
 *   computed and added to the region. this deviation is given as convexes containing the
 *   estimated and exact border
 * 
 * the 2 sets of dirty squares are then merged and give the final region
 */

#ifndef REGION_H_
#define REGION_H_

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "Image.h"
#include "MPoint.h"
#include "MArray.h"

class Region 
{
private:
	Image*		_inside; // inside part of the dirty region
	Image*		_border; // border part of the dirty region
	
	int			_sizeH, _sizeV; // pixels covered by the region
	float		_scale; // size of a dirty square
	int			_sSizeH, _sSizeV; // 'bitmap' sizes, ie _sizeH and _sizeV divided by _scale
	
	/*
	 * each convex is converted in dirty square spans by computing leftmost and rightmost
	 * square for each line in the [0 ; _sSizeV] range
	 * these 2 arrays store the leftmost and rightmost positions
	 */
	MArray<int>	_convexLeft;
	MArray<int>	_convexRight;
	
public:
	Region(int sizeH, int sizeV, float scale);
	virtual ~Region();
	
	/*
	 * return the image representing the dirty squares
	 * (val = 0x00 <=> dirty
	 *  val = 0xff <=> untouched)
	 */
	Image*		dirtyRegion(void) {return _border;};
	
	/*
	 * callback for the dirtying by a path: adding an approximate edge to the
	 * border
	 */
	void		insideEdge(MPoint<float> const &a, MPoint<float> const &b);
	/*
	 * callback for the dirtying by a path: adding a convex, defind by 2, 3 or 4 points
	 */
	void		borderLine(MPoint<float> const &a, MPoint<float> const &b);
	void		borderTriangle(MPoint<float> const &a, MPoint<float> const &b, MPoint<float> const &c);
	void		borderQuad(MPoint<float> const &a, MPoint<float> const &b, MPoint<float> const &c, MPoint<float> const &d);

	/*
	 * once all edges and convexes have been added, finalize the region
	 */
	void		finish(void);
	
private:
	/*
	 * merge dirty square of the inside part with the ones from the convexes
	 */
	void		mergeInside(void);
	
	/*
	 * handling of a single convex:
	 * - erase leftmost/rightmost arrays
	 * - add an edge
	 * - add a span on a line
	 * - flush to the image storing all convexes
	 */
	void		convexInit(void);
	void		convexAdd(MPoint<float> const &a, MPoint<float> const &b);
	void		convexSpanAdd(int y, float a, float b);
	void		convexFlush(void);
};

#endif /*REGION_H_*/
