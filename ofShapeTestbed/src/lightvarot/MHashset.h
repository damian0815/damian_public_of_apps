/*
 * a very basic hashset implementation
 * 
 * uses an array allocated like the MArray template
 * 
 * implementation uses bucket lists, and the 'next' pointer for the bucket linked list
 * has expected to be stored directly in the data, in the for of an index in the data
 * array. this index is set/retrieved with the setNext()/getNext() functions
 */

#ifndef my_hashset
#define my_hashset

#include <stdlib.h>
#include <stdio.h>

#include "MHashor.h"

template <class T> class MHashset {
private:
	// hash function
	MHashor<T>		*_hashor;
	bool			_ownsHash; // is the hashset responsible for deallocating the hash
	                           // function?
	
	// data array
	int             _nb, _max;
	T*				_data;
	
	// array for the heads of the linked lists
	int             _size;
	int*			_head;

public:
	MHashset(MHashor<T> *func, bool owns) {
		_hashor = func;
		_ownsHash = owns;
		_size = _nb = _max = 0;
		_data = NULL;
		_head = NULL;
	};
	virtual ~MHashset(void) {
		destroyData();
		destroyHead();
		if ( _ownsHash ) delete _hashor;
	};
	
	/*
	 * accessors
	 */
	int		nb(void) const {return _nb;};
	T*		data(void) const {return _data;};
	T&		operator[](T const &i) {
		int	k;
		int	p = find(i, k);
		if ( p >= 0 ) {
			return _data[p];
		} else {
			char*	exStr = new char[256];
			sprintf(exStr, "[] hashset error: %i not in [0 ; %i]", p, _nb);
			throw exStr;
		}
	}
private:
	/*
	 * cleaning
	 */
	void destroyData(void) {
		if ( _data != NULL ) delete [] _data;
		_data = NULL;
	}
	void destroyHead(void) {
		if ( _head != NULL ) delete [] _head;
		_head = NULL;
	}
	/*
	 * resize the data array
	 */
	void resizeData(int to) {
		if ( to > _max ) {
			_max = to;
			T* ndata = new T[_max](); 
			for (int i=0;i<_nb;i++) {
				ndata[i] = _data[i];
			}
			destroyData();
			_data = ndata;
		}
	}
	/*
	 * resize the linked lists heads array
	 */
	void resizeHead(int to) {
		if ( _size < to ) {
			_size = to;
			int* nhead = new int [_size];
			destroyHead();
			_head = nhead;
			// since the heads array change, we need to re-hash everything
			for (int i=0;i<_size;i++) _head[i] = -1;
			for (int i=0;i<_nb;i++) _data[i].setNext(-1);
			for (int i=0;i<_nb;i++) reinsert(i);
		}
	}
	/*
	 * compute the key
	 */
	int key(const T &x) {
		int k = _hashor->hash(x);
		if ( _size <= 0 ) return 0;
		k = k % _size;
		return k;
	}
	/*
	 * (re)insert an element in the bucket lists
	 */
	void reinsert(int pos, int key) {
		int oldHead = _head[key];
		_head[key] = pos;
		_data[pos].setNext(oldHead);
		 
	}
	void reinsert(int pos) {
		reinsert(pos, key(_data[pos]));
	}
	/*
	 * add an element in the data array
	 */
	int append(const T &x, int key) {
		if ( _nb >= _max ) resizeData(2*_nb + 1);
		int pos = _nb;
		_nb++;
		_data[pos] = x;
		reinsert(pos, key);
		return pos;
	}
	int append(const T &x) {
		return append(x, key(x));
	}
	/*
	 * find an element, and return the key in the process
	 * return the index in the data array, or -1 if not found
	 */
	int find(const T &x, int &k) {
		k = key(x);
		if ( _nb <= 0 ) return -1;
		int cur = _head[k];
		while ( cur >= 0 && cur < _nb && !(_hashor->equals(_data[cur],x)) ) cur = _data[cur].getNext();
		return cur;
	}
public:
	/*
	 * add an element in the hashset
	 */
	int put(const T &x) {
		int k;
		int pos = find(x,k);
		// if data preexists, we replace the data in the array with the new one
		if ( pos >= 0 ) {
			int oldNext = _data[pos].getNext();
			_data[pos] = x;
			_data[pos].setNext(oldNext);
			return pos;
		}
		// if load > 75%, double the head array size
		int load = ((_nb + 1) * 4 ) / 3 ;
		if ( _size < load ) {
			resizeHead(2 * (_nb + 1));
			k = key(x);
		}
		return append(x, k);
	}
	/*
	 * test presence of an element
	 */
	bool contains(const T &x) {
		int k;
		int pos = find(x,k);
		if ( pos >= 0 ) return true;
		return false;
	}
	/*
	 * misc
	 */
	void dump(void) {
		printf("nb=%i (max=%i) size=%i\n", _nb, _max, _size);
		for (int i=0;i<_nb;i++) _data[i].dump();
		printf("\n");
		for (int i=0;i<_size;i++) printf("%i->%i ", i, _head[i]);
		printf("\n");
	}
};

#endif
