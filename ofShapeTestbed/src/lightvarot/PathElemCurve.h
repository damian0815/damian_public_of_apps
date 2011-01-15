#ifndef PATHELEMCURVE_H_
#define PATHELEMCURVE_H_

#include "PathElem.h"
#include "MPoint.h"

class PathElemCurve : public PathElem
{
public:
	MPoint<float>			_st;
	MPoint<float>			_p1;
	MPoint<float>			_p2;
	MPoint<float>			_en;
	
	PathElemCurve();
	PathElemCurve(MPoint<float> const &st
			    , MPoint<float> const &p1
			    , MPoint<float> const &p2
			    , MPoint<float> const &en, Path* dad);
	PathElemCurve(PathElemCurve const &o);
	virtual ~PathElemCurve();
	
	virtual PathElem*	clone(void) const;
	
	virtual int			type(void) const;
	virtual bool		equals(PathElem const *o) const;
	virtual	void		dump(void) const;

	virtual	bool		hasEnd();
	virtual	MPoint<float>	endPoint();
	
	PathElemCurve*	subPathStart(float s);
	PathElemCurve*	subPathEnd(float s);
	virtual	PathElem*	subPathElem(float st, float en);
	virtual void		appendSubPathElem(Path* to, float st, float en, MPoint<float> exact);
	
	virtual	void		convert(Polyline * to, float delta);
	void		convertCurve(Polyline* to, float delta, MPoint<float> const &p0
			, MPoint<float> const &p1
			, MPoint<float> const &p2
			, MPoint<float> const &p3
			, float p0S, float p3S);
	virtual	void		dirty(Region * to, float delta);
	void		dirtyCurve(Region* to, float delta, MPoint<float> const &p0
			, MPoint<float> const &p1
			, MPoint<float> const &p2
			, MPoint<float> const &p3
			, float p0S, float p3S);

	virtual	bool		hasEndTangent();
	virtual	MPoint<float>	endTangent();
	virtual	bool		hasStartTangent();
	virtual	MPoint<float>	startTangent();
	/*
	 * offset/inset of current path
	 */
	virtual void	offset(Path* to, float offset, float delta, bool inSubpath, MPoint<float> const &firstT, MPoint<float> const &lastT, MPoint<float> const &lastP);
	void	offset(Path* to, float offset, float delta, float stS, float enS);
};

#endif /*PATHELEMCURVE_H_*/
