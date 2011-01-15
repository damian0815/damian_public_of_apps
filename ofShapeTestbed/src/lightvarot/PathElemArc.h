#ifndef PATHELEMARC_H_
#define PATHELEMARC_H_

#include "PathElem.h"
#include "MPoint.h"

class PathElemArc : public PathElem
{
public:
	MPoint<float>			_st;
	MPoint<float>			_center;
	MPoint<float>			_en;
	bool					_clockwise;
	
	PathElemArc();
	PathElemArc(MPoint<float> const &st, MPoint<float> const &center, MPoint<float> const &en, bool clockwise, Path* dad);
	PathElemArc(PathElemArc const &o);
	virtual ~PathElemArc();
	
	virtual PathElem*	clone(void) const;
	
	virtual int			type(void) const;
	virtual bool		equals(PathElem const *o) const;
	virtual	void		dump(void) const;
	
	virtual	bool		hasEnd();
	virtual	MPoint<float>	endPoint();
	
	virtual	PathElem*	subPathElem(float st, float en);
	virtual void		appendSubPathElem(Path* to, float st, float en, MPoint<float> exact);
	
	virtual	void		convert(Polyline * to, float delta);
	virtual	void		dirty(Region * to, float delta);

	virtual	bool		hasEndTangent();
	virtual	MPoint<float>	endTangent();
	virtual	bool		hasStartTangent();
	virtual	MPoint<float>	startTangent();
	/*
	 * offset/inset of current path
	 */
	virtual void	offset(Path* to, float offset, float delta, bool inSubpath, MPoint<float> const &firstT, MPoint<float> const &lastT, MPoint<float> const &lastP);
};

#endif /*PATHELEMARC_H_*/
