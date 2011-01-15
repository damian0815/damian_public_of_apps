#ifndef PATHELEMCLOSE_H_
#define PATHELEMCLOSE_H_

#include "PathElem.h"
#include "MPoint.h"

class PathElemClose : public PathElem
{
public:
	PathElemClose();
	PathElemClose(Path* dad);
	PathElemClose(PathElemClose const &o);
	virtual ~PathElemClose();
	
	virtual PathElem*	clone(void) const;
	
	virtual int			type(void) const;
	virtual bool		equals(PathElem const *o) const;
	virtual	void		dump(void) const;
	
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

#endif /*PATHELEMCLOSE_H_*/
