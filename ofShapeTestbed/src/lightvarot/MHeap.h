/*
 * template for making binary heaps
 */

#ifndef my_binary_heap
#define my_binary_heap

#include <stdlib.h>
#include <stdio.h>

#include "MComparator.h"

template <class T> class MHeap {
private:
	int             _nb, _max; // number of datas used, and allocated respectively
	T*				_data; // the data array proper
	MComparator<T>*	_cmp;
public:
	/*
	 * creates an empty heap
	 */
	MHeap(MComparator<T>* cmp) {
		_cmp = cmp;
		_nb = _max = 0;
		_data = NULL;
	};
	virtual ~MHeap(void) {
		destroyData();
	};
	
	/*
	 * accessors
	 */
	int  nb(void) const {
		return _nb;
	};
	/*
	 * cleaning
	 */
	void destroyData(void) {
		if ( _data != NULL ) delete [] _data;
		_data = NULL;
	};
	/*
	 * changes the allocated size
	 * only increases
	 */
	void resize(int to) {
		if ( to > _max ) {
			_max = to;
			T* ndata = new T[_max](); 
			for (int i=0;i<_nb;i++) ndata[i] = _data[i];
			destroyData();
			_data = ndata;
		}
	};
	/*
	 * adding
	 */
	void offer(const T &x) {
		if ( _nb >= _max ) resize(2*_nb +1);
		int pos = _nb;
		_nb++;
		_data[pos] = x;
		percolateUp(pos);
	};
	/*
	 * suppression of element, given by value or by index
	 * if given by value, then the heap is searched brutally
	 * (a potential speedup would be to have each element store
	 * its index in the queue and maintain this index in the 
	 * percolation routines)
	 */
	void remove(T const &x) {
		for (int i=0;i<_nb;i++) {
			if ( _cmp->equals(_data[i], x) ) {
				remove(i);
				break;
			}
		}
	}
	void remove(int no) {
		if ( no >= 0 && no < _nb ) {
			_nb --;
			if ( no < _nb ) {
				_data[no] = _data[_nb];
				percolateUpDown(no);
			}
		} else {
			char*	exStr = new char[256];
			sprintf(exStr, "remove array error: %i not in [0 ; %i]", no, _nb);
			throw exStr;
		}
	};
	/*
	 * retrieve the smallest element in the heap
	 */
	T	poll(void) {
		if ( _nb > 0 ) {
			T	res = _data[0];
			_nb --;
			if ( 0 < _nb ) {
				_data[0] = _data[_nb];
				percolateDown(0);
			}
			return res;
		} else {
			char*	exStr = new char[256];
			sprintf(exStr, "poll error: empty heap");
			throw exStr;
		}
	};
	/*
	 * for debugging purposes
	 */
	void dump(void) {
		printf("nb=%i (max=%i)\n", _nb, _max);
		int	st = 1, en = 2;
		while ( st <= _nb ) {
			for (int i = st-1;i<en-1;i++) {
				if ( i < _nb ) _data[i]->dump();
			}
			printf("\n");		
			st *=2;
			en *=2;
		}
	};
private:
	/*
	 * utility
	 */
	void	swap(int a, int b) {
		if ( a == b ) return;
		T	s = _data[a];
		_data[a] = _data[b];
		_data[b] = s;
	};
	/*
	 * percolation routines
	 */
	void	percolateDown(int pos) {
		while (pos < _nb ) {
			int	left = 2 * pos + 1;
			int	right = 2 * pos + 2;
			if ( left >= _nb ) return;
			if ( right >= _nb ) {
				if ( _cmp->compare(_data[pos], _data[left]) > 0 ) swap(pos, left);
				return;
			}
			int	min = ( _cmp->compare(_data[left], _data[right]) < 0 ) ? left : right;
			if ( _cmp->compare(_data[pos], _data[min]) > 0 ) {
				swap(pos, min);
				pos = min;
			} else {
				return;
			}
		}
	};
	void	percolateUp(int pos) {
		while (pos > 0 ) {
			int	up = (pos - 1) / 2;
			if ( _cmp->compare(_data[pos], _data[up]) < 0 ) {
				swap(pos, up);
				pos = up;
			} else {
				return;
			}
		}
	};
	void	percolateUpDown(int pos) {
		if ( pos > 0 ) {
			int	up = (pos - 1) / 2;
			if ( _cmp->compare(_data[pos], _data[up]) <= 0 ) {
				percolateUp(pos);
			} else {
				percolateDown(pos);
			}
		} else {
			percolateDown(pos);
		}
	};
};


#endif
