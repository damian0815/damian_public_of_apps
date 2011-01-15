/*
 * function for hashing:
 * - hash function
 * - equality test for lookup
 */

#ifndef my_hash_func
#define my_hash_func

template <class T> class MHashor {
public:
	MHashor(void) {};
	virtual ~MHashor(void) {};
	
	virtual	bool	equals(T const &a, T const &b) const {return true;};
	virtual	int		hash(T const &a) const {return 0;};
};

#endif
