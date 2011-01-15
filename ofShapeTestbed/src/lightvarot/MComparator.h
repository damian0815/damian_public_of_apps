/*
 * comparator class for sorting
 * 
 * implements comparison and equality test, in case one wants to be more clever
 * about equality than a mere 'compare() == 0'
 * 
 */

#ifndef my_comparator
#define my_comparator

template <class T> class MComparator {
public:
	MComparator(void) {};
	virtual ~MComparator(void) {};
	
	virtual	bool	equals(T const &a, T const &b) const {return true;};
	virtual	int		compare(T const &a, T const &b) const {return 0;};
};

#endif
