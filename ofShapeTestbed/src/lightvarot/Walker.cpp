#include "Walker.h"

int	AdjacencyCmpByPoint::compare(WindingAdjacency const &a, WindingAdjacency const &b) const {
	if ( a._pt < b._pt ) return -1;
	if ( a._pt > b._pt ) return 1;
	int		pt = a._pt;
	WindingEdge	ae = *(*_edges + a._edge);
	WindingEdge	be = *(*_edges + b._edge);
	bool	aout = ( ae._stP == pt );
	bool	bout = ( be._stP == pt );
	FPoint	ad = (aout)? ae._dir: -ae._dir;
	FPoint	bd = (bout)? be._dir: -be._dir;
	int aq = 0, bq = 0;
	if ( ad._y < 0 ) {
		if ( ad._x < 0 ) {
			aq = 2;
		} else {
			aq = 3;
		}
	} else {
		if ( ad._x < 0 ) {
			aq = 1;
		} else {
			aq = 0;
		}
	}
	if ( bd._y < 0 ) {
		if ( bd._x < 0 ) {
			bq = 2;
		} else {
			bq = 3;
		}
	} else {
		if ( bd._x < 0 ) {
			bq = 1;
		} else {
			bq = 0;
		}
	}
	if ( aq < bq ) return -1;
	if ( aq > bq ) return 1;
	double	cr = ad ^ bd;
	if ( cr > 0 ) return -1;
	if ( cr < 0 ) return 1;
	if ( aout && !bout ) return -1;
	if ( !aout && bout ) return 1;
	// departager selon id d'arete, et attention au sens
	// car on veut eviter les inversions entre 2 aretes colineaires
	if ( aout ) {
		if ( a._edge < b._edge ) return -1;
		if ( a._edge > b._edge ) return 1;
	} else {
		if ( a._edge < b._edge ) return 1;
		if ( a._edge > b._edge ) return -1;
	}
	return 0;
}
int WindingEdge::other(int adj) {
	if ( adj == _stA ) return _enA;
	if ( adj == _enA ) return _stA;
	return -1;
}
int WindingEdge::otherFromPoint(int pt) {
	if ( pt == _stP ) return _enA;
	if ( pt == _enP ) return _stA;
	return -1;
}
void WindingEdge::setWinding(int at, bool left, int val0, int val1, WindingEdge* from) {
	_prev = from;
	_visited = true;
	if ( at == _stA ) {
		if ( left ) {
			if ( _group == 0 ) {
				_left0 = val0;
				_right0 = val0 + 1;
				_left1 = val1;
				_right1 = val1;
			} else if ( _group == 1 ) {
				_left0 = val0;
				_right0 = val0;
				_left1 = val1;
				_right1 = val1 + 1;
			}
		} else {
			if ( _group == 0 ) {
				_left0 = val0 - 1;
				_right0 = val0;
				_left1 = val1;
				_right1 = val1;
			} else if ( _group == 1 ) {
				_left0 = val0;
				_right0 = val0;
				_left1 = val1 - 1;
				_right1 = val1;
			}
		}
	} else if ( at == _enA ) {
		if ( left ) {
			if ( _group == 0 ) {
				_left0 = val0 - 1;
				_right0 = val0;
				_left1 = val1;
				_right1 = val1;
			} else if ( _group == 1 ) {
				_left0 = val0;
				_right0 = val0;
				_left1 = val1 - 1;
				_right1 = val1;
			}
		} else {
			if ( _group == 0 ) {
				_left0 = val0;
				_right0 = val0 + 1;
				_left1 = val1;
				_right1 = val1;
			} else if ( _group == 1 ) {
				_left0 = val0;
				_right0 = val0;
				_left1 = val1;
				_right1 = val1 + 1;
			}
		}
	} else {
		
	}
}
int WindingEdge::getWinding0(int at, bool left) {
	if ( at == _stA ) {
		if ( left ) {
			return _left0;
		} else {
			return _right0;
		}
	} else if ( at == _enA ) {
		if ( left ) {
			return _right0;
		} else {
			return _left0;
		}
	} else {
		
	}
	return 0;
}
int WindingEdge::getWinding1(int at, bool left) {
	if ( at == _stA ) {
		if ( left ) {
			return _left1;
		} else {
			return _right1;
		}
	} else if ( at == _enA ) {
		if ( left ) {
			return _right1;
		} else {
			return _left1;
		}
	} else {
		
	}
	return 0;
}

Walker::Walker(MArray<WindingEdge> const &ed, MArray<WindingAdjacency> const &ad) : _edges(ed + 0, ed.nb())
       																	          , _adjs(ad + 0, ad.nb()) {
	
}
Walker::~Walker() {
	
}

void			Walker::walk(bool clockwise) {
	int startAdj = -1;
	do {
		bool found = false;
		for (int i=startAdj+1;i<_adjs.nb();i++) {
			if ( _adjs[i]._visited == false ) {
				startAdj = i;
				found = true;
				break;
			}
		}
		if ( found == false ) break;
		
		startAdj = newConnectedComponent(startAdj);
		
		int curAdj = startAdj;
		WindingEdge* curWind = NULL;
		do {
			int wNo = _adjs[curAdj]._edge;
			_adjs[curAdj]._visited = true;
			if ( _edges[wNo]._visited ) {
				do {
					int nAdj = curAdj;
					int ptNo = _adjs[curAdj]._pt;
					do {
						nAdj = circleAtPoint(nAdj, ptNo, clockwise);
						if ( _edges[_adjs[nAdj]._edge]._visited == false )  break;
					} while ( nAdj != curAdj );
					if ( nAdj == curAdj ) {
						preBacktrack(curAdj, _adjs[curAdj]._edge, clockwise, curWind);
						curAdj = curWind->otherFromPoint(ptNo);
						postBacktrack(curAdj, _adjs[curAdj]._edge, clockwise, curWind);
						curWind = curWind->_prev;
					} else {
						curAdj = nAdj;
						advanceAtPoint(curAdj, _adjs[curAdj]._edge, clockwise, curWind);
						break;
					}
				} while (curWind != NULL );
			} else {
				preAdvanceAtEdge(curAdj, wNo, clockwise, curWind);
				curWind = _edges + wNo;
				_edges[wNo]._visited = true;
				curAdj = curWind->other(curAdj);
				postAdvanceAtEdge(curAdj, wNo, clockwise, curWind);
			}
		} while ( curWind != NULL );
	} while ( true );
}

int		Walker::newConnectedComponent(int startAdj) {
	return startAdj;
}
int     Walker::circleAtPoint(int nAdj, int ptNo, bool clockwise) {
	if ( clockwise ) {
		nAdj++;
		if ( nAdj >= _adjs.nb() || _adjs[nAdj]._pt != ptNo ) {
			nAdj--;
			while ( nAdj >= 0 && _adjs[nAdj]._pt == ptNo ) nAdj--;
			nAdj++;
		}
	} else {
		nAdj--;
		if ( nAdj < 0 || _adjs[nAdj]._pt != ptNo ) {
			nAdj++;
			while ( nAdj < _adjs.nb() && _adjs[nAdj]._pt == ptNo ) nAdj++;
			nAdj--;
		}
	}
	return nAdj;
}
void	Walker::preBacktrack(int curAdj, int wNo,  bool clockwise, WindingEdge* curWind) {
	
}
void	Walker::postBacktrack(int curAdj, int wNo,  bool clockwise, WindingEdge* curWind) {
	
}
void	Walker::advanceAtPoint(int curAdj, int wNo,  bool clockwise, WindingEdge* curWind) {
	
}
void	Walker::preAdvanceAtEdge(int curAdj, int wNo,  bool clockwise, WindingEdge* curWind) {
	
}
void	Walker::postAdvanceAtEdge(int curAdj, int wNo,  bool clockwise, WindingEdge* curWind) {
	
}

