/*
 * a command in a Path
 * 
 * subclasses of this class do the actual curve -> polyline conversion
 */

#ifndef PATHELEM_H_
#define PATHELEM_H_

#include <stdlib.h>
#include <math.h>
#include "MPoint.h"

class Path;
class Polyline;
class Region;

/*
 * types of commands
 */
enum {
	elem_none,
	elem_move,
	elem_line,
	elem_curve,
	elem_arc,
	elem_close
};

class PathElem
{
public:
	Path*		_owner; // who owns this command
	
	PathElem();
	PathElem(Path* dad);
	PathElem(PathElem const &o);
	virtual ~PathElem();
	
	/*
	 * cloning a command
	 */
	virtual PathElem*	clone(void) const;
	/*
	 * cloning a command and setting the clone's owner
	 */
	virtual PathElem*	clone(Path* dad) const;
	
	/*
	 * access to the type of command
	 */
	virtual int			type(void) const;
	/*
	 * equality test (not used)
	 */
	virtual bool		equals(PathElem const *o) const;
	/*
	 * misc. for debug
	 */
	virtual	void		dump(void) const;
	
	/*
	 * endpoint of the path command retrieval
	 */
	virtual	bool		hasEnd();
	virtual	MPoint<float>	endPoint();
	
	/*
	 * returns a command corresponding to a portion of the path described by this command
	 * the result is the path between the  position 'st' and 'en' (this command starts at
	 * position 0 and ends at position 1)
	 */
	virtual	PathElem*	subPathElem(float st, float en);
	/*
	 * returns a command corresponding to a portion of the path described by this command
	 * and sets the result's owner, as well as its endpoint
	 */
	virtual void		appendSubPathElem(Path* to, float st, float en, MPoint<float> exact);
	
	/*
	 * conversion to polyline
	 * the result should be guaranted to be within 'delta' of the exact path
	 */
	virtual	void		convert(Polyline * to, float delta);
	/*
	 * conversion to polyline+convexes for the edges
	 */
	virtual	void		dirty(Region * to, float delta);
	/*
	 * info needed for offseting/inseting paths
	 */
	virtual	bool		hasEndTangent();
	virtual	MPoint<float>	endTangent();
	virtual	bool		hasStartTangent();
	virtual	MPoint<float>	startTangent();
	/*
	 * offset/inset of current path
	 */
	virtual void	offset(Path* to, float offset, float delta, bool inSubpath, MPoint<float> const &firstT, MPoint<float> const &lastT, MPoint<float> const &lastP);
};

#endif /*PATHELEM_H_*/
