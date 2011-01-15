/*
 * template for making arrays
 * 
 * this implementation allocates using new T[] and delete [], meaning:
 * - datas must have an empty constructor
 * - datas must implement the = operator (or copy constructor)
 */

#ifndef my_array
#define my_array

#include <stdlib.h>
#include <stdio.h>

#include "MComparator.h"

template <class T> class MArray {
private:
	bool			_owns;	// ownership of the data array. if not owner, then the array
	                        // destructor will not attempt to delete the data array
	int             _nb, _max; // number of datas used, and allocated respectively
	T*				_data; // the data array proper
	
public:
	/*
	 * creates an empty array
	 */
	MArray(void) {
		_nb = _max = 0;
		_data = NULL;
		_owns = true;
	};
	/*
	 * creates an array and pre-allocates nb datas, but array size remains 0
	 */
	MArray(int nb) {
		_nb = 0;
		_max = nb;
		_data = new T[_max]();
		_owns = true;
	};
	/*
	 * creates an array and pre-allocates nb datas
	 * if initAsFull = false, then the array size is 0
	 * if initAsFull = true, then the array size is nb too
	 */
	MArray(int nb, bool initAsFull) {
		_nb = (initAsFull)?nb : 0;
		_max = nb;
		_data = new T[_max]();
		_owns = true;
	};
	/*
	 * inits the array with external data
	 * the array will not assume ownership of the data
	 * if the array size has to changes, another array is allocated, and the old one
	 * forgotten
	 */
	MArray(T* data, int nb) {
		_nb = nb;
		_max = nb;
		_data = data;
		_owns = false;
	};
	/*
	 * copies an array
	 */
	MArray(MArray<T> const &o) {
		_nb = o._nb;
		_max = o._max;
		_data = new T[_max]();
		for (int i=0;i<_nb;i++) _data[i] = o._data[i];
		_owns = true;
	};
	virtual ~MArray(void) {
		destroyData();
	};
	
	/*
	 * accessors
	 */
	int  nb(void) const {
		return _nb;
	};
	T*   data(void) const {
		return _data;
	};
	T&	 operator[](int const i) const {
		if ( i < 0 || i >= _nb ) {
			// don't put much effort
			char*	exStr = new char[256];
			sprintf(exStr, "[] array error: %i not in [0 ; %i[", i, _nb);
			throw exStr;
		}
		return _data[i];
	};
	/*
	 * cleaning
	 */
	void destroyData(void) {
		if ( _owns == true ) {
			if ( _data != NULL ) delete [] _data;
		}
		_data = NULL;
		_owns = true;
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
	 * set the array size to some SMALLER size, without changing the allocated size
	 */
	void truncate(int to) {
		if ( to >= 0 && to <= _nb ) {
			_nb = to;
		}
	};
	/*
	 * adding
	 */
	int append(const T &x) {
		if ( _nb >= _max ) resize(2*_nb +1);
		int pos = _nb;
		_nb++;
		_data[pos] = x;
		return pos;
	};
	/*
	 * looking up an element
	 */
	int	find(const T &x, MComparator<T> const &func) {
		for (int i=0;i<_nb;i++) {
			if ( func.equals(_data[i], x) ) return i;
		}
		return -1;
	}
	/*
	 * suppression of element at index no
	 */
	void remove(int no) {
		if ( no >= 0 && no < _nb ) {
			_nb --;
			if ( no < _nb ) _data[no] = _data[_nb];
		} else {
			char*	exStr = new char[256];
			sprintf(exStr, "remove array error: %i not in [0 ; %i]", no, _nb);
			throw exStr;
		}
	};
	/*
	 * suppression of element x
	 * only removes the first found
	 */
	void remove(const T &x, MComparator<T> const &func) {
		int no = find(x, func);
		if ( no >= 0 ) remove(no);
	};
	/*
	 * swaps for sorting
	 */
	// a <- b
	// b <- a
	void swap(int a, int b) {
		if ( a != b ) {
			T tmp = _data[a];
			_data[a] = _data[b];
			_data[b] = tmp;
		}
	};
	// a <- b
	// b <- c
	// c <- a
	void swap(int a, int b, int c) {
		if ( a == b ) {
			swap(a,c);
		} else if ( a == c ) {
			swap(a,b);
		} else if ( b == c ) {
			swap(a,b);
		} else {
			T tmp = _data[a];
			_data[a] = _data[b];
			_data[b] = _data[c];
			_data[c] = tmp;
			
		}
	};
	/*
	 * quicksort (in-place)
	 */
	void sort(int st, int en, MComparator<T> const &func) {
		if ( st >= en ) return;
		if ( st+1 == en ) {
			int cmp = func.compare(_data[st], _data[en]);
			if ( cmp > 0 ) swap(st, en);
			return;
		}
		int mid = (st + en) / 2;
		T	pivot = _data[mid];
		int lmid = mid, rmid = mid;
		int left = st, right = en;
		while ( left < lmid && right > rmid ) {
			int lcmp = func.compare(_data[left], pivot);
			int rcmp = func.compare(_data[right], pivot);
			if ( lcmp < 0 ) {
				if ( rcmp > 0 ) {
					// gauche ok, droite ok
					left++;
					right--;
				} else if ( rcmp == 0 ) {
					// gauche ok, droite a coller au milieu
					swap(rmid+1, right);
					left++;
					rmid++;
				} else  {
					// gauche ok, droite a changer de coté
					if ( left+1 < lmid ) {
						// il y a la place pour swapper la droite avec un element de gauche
						swap(left+1, right);
						left+=2;
					} else {
						// pas de place, il faut decaler le milieu a droite
						swap(lmid, right, rmid+1);
						left+=2;
						lmid++;
						rmid++;
					}
				}
			} else if ( lcmp == 0 ) {
				if ( rcmp > 0 ) {
					// gauche a coller au milieu, droite ok
					swap(left, lmid-1);
					lmid--;
					right--;
				} else if ( rcmp == 0 ) {
					// gauche et droite a coller ua milieu
					swap(left, lmid-1);
					swap(rmid+1, right);
					rmid++;
					lmid--;
				} else  {
					// gauche a coller au milieu et droite a changer de coté
					if ( left < lmid-1 ) {
						// il y a la place a gauche
						swap(left, right, lmid-1);
						left++;
						lmid--;
					} else {
						// pas de place, il faut decaler le milieu
						swap(left, right, rmid+1);
						left++;
						rmid++;
					}
				}
			} else  {
				if ( rcmp > 0 ) {
					// gauche a changer de cote, droite ok
					if ( right-1 > rmid ) {
						// il y a la place pour swapper
						swap(left, right-1);
						right-=2;
					} else {
						// il faut decaler le milieu
						swap(rmid, left, lmid-1);
						right-=2;
						lmid--;
						rmid--;
					}
				} else if ( rcmp == 0 ) {
					// gauche a cahnger de cote, droite a coller au milieu
					if ( right > rmid+1 ) {
						// on peut tout mettre a droite
						swap(right, left, rmid+1);
						right--;
						rmid++;
					} else {
						// il faut decaler le milieu
						swap(right, left, lmid-1);
						right--;
						lmid--;
					}
				} else  {
					// gauche et droite pas ok, on echange
					swap(left, right);
					left++;
					right--;
				}
			}
		}
		while ( left < lmid ) {
			int lcmp = func.compare(_data[left], pivot);
			if ( lcmp < 0 ) {
				// gauche ok
				left++;
			} else if ( lcmp == 0 ) {
				// a coller au milieu
				swap(left, lmid-1);
				lmid--;
			} else {
				// a echanger en decalant le milieu
				swap(rmid, left, lmid-1);
				rmid--;
				lmid--;
			}
		}
		while ( right > rmid ) {
			int rcmp = func.compare(_data[right], pivot);
			if ( rcmp > 0 ) {
				// droite ok
				right--;
			} else if ( rcmp == 0 ) {
				// a coller au milieu
				swap(right, rmid+1);
				rmid++;
			} else {
				// a echanger en decalant le milieu
				swap(lmid, right, rmid+1);
				rmid++;
				lmid++;
			}
		}
		sort(st, lmid-1, func);
		sort(rmid+1, en, func);
	};
	/*
	 * affichage
	 */
	void dump(void) {
		printf("nb=%i (max=%i)\n", _nb, _max);
		for (int i=0;i<_nb;i++) _data[i].dump();
		printf("\n");		
	};
};

// accessor
template <class T> T* operator+(MArray<T> const &a, int i) {
	return a.data() + i;
};

#endif
