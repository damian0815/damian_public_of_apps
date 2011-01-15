/*
 * a path, defined by drawing commands: move point, straight line, cubic bezier patch,
 * circle patch
 * 
 * the commands are stored in an array of PathElem that the Path instance assumes it owns
 * and deletes in the destructor
 * 
 * path to polygon is handled by means of a Polyline instance: the Path will feed it with
 * moveto/lineto/close commands
 */

#ifndef PATH_H_
#define PATH_H_

#include "PathElem.h"
#include "MArray.h"
#include "MPoint.h"

class Polyline;
class Region;

class Path
{
private:
	MArray<PathElem*>	_elems; // commands

	bool				_firstPtSet; // utilities for path construction
	MPoint<float>		_firstPt;
	bool				_lastPtSet;
	MPoint<float>		_lastPt;

public:
	Path();
	Path(Path const &o); // copies all the commands of the 'o' path
	virtual ~Path();
	
	/*
	 *  drawing commands
	 */
	void		moveto(MPoint<float> pt);
	void		lineto(MPoint<float> pt);
	void		curveto(MPoint<float> p1, MPoint<float> p2, MPoint<float> pt);
	void		arcto(MPoint<float> center, MPoint<float> pt, bool clockwise);
	void		close(void);

	/*
	 * just in case
	 */
	void		moveto(MPoint<double> pt) {moveto(MPoint<float>(pt._x, pt._y));};
	void		lineto(MPoint<double> pt) {lineto(MPoint<float>(pt._x, pt._y));};
	/*
	 *  transform the path in a polygon and feeds it to the Polyline
	 */
	void		fill(Polyline* to, float delta);
	
	/*
	 *  transform the path in an approximate polygon and feeds it to the Region
	 * the difference with fill() is that it makes a coarse polyline and gives
	 * a set of convexes for to cover the edges. the result polygon + the convexes
	 * covers the Path (with non-zero winding rule) 
	 */
	void		dirty(Region* to, float delta);
	
	/*
	 * make an offset of the path, more exactly the border of what would be a stroke
	 * of the path
	 * it's not *really* an offset since it applies to paths and not shapes. the best
	 * way to describe it, is that the result is the path you would get by 'pushing'
	 * on one side of the path. for offset > 0, you push on the left side, and offset < 0
	 * you push on the right side. if the path is already the contour of a shape, then
	 * 'inside' should be always the right side, and then offset > 0 is actually inset
	 */
	Path*		offset(float offset, float delta);
	/*
	 *  misc.
	 */
	void		dump(void);
};

#endif /*PATH_H_*/
