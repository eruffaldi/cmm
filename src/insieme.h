#ifndef MYSET_H
#define MYSET_H

#include "btree.h"

// insieme, dalla definizione STL
// The template class describes an object that controls a varying-length sequence of 
// elements of type const Key. Each element serves as both a sort key and a value. 
// The sequence is represented in a way that permits lookup, insertion, and removal 
// of an arbitrary element with a number of operations proportional to the logarithm 
// of the number of elements in the sequence (logarithmic time). Moreover, inserting 
// an element invalidates no iterators, and removing an element invalidates only those 
// iterators that point at the removed element.


template <class Key, class Cmp = less<Key> >
class insieme
{
public:
	typedef Key value_type;
	typedef binTree< Key, Cmp> tree;
	typedef tree::iterator iterator;	

protected:
	tree _tree;
public:
	insieme() {}	
	insieme(const insieme & t) : _tree(t._tree) {}
	void erase(const Key& k) { _tree.erase(k);}
	void erase(iterator it) { _tree.erase(it);}
	void erase(iterator it1, iterator it2) { _tree.erase(it1, it2);}
	iterator find(const Key&k) {return _tree.find(k);}
	void clear() {return _tree.clear();}
	int size() const {return _tree.size();}
	pair<iterator, bool> insert(const Key& x) {return _tree.insert(x);}	
	iterator begin() {return _tree.begin();}
	iterator root() {return _tree.root();}
	iterator end() {return _tree.end();}
	insieme & operator *= (const insieme &);	// intersection
	insieme & operator += (const insieme &);	// union
	insieme & operator -= (const insieme &);	// difference
	insieme operator+(const insieme &) const;	
	insieme operator-(const insieme &) const;	
	insieme operator*(const insieme &) const;	
};



template <class Key, class Cmp>
insieme<Key,Cmp> & insieme<Key,Cmp>::operator -= (const insieme & tw2)
{
	// tolgo ogni elemento di t1 presente in t2
	iterator it;
	insieme & t2 = *(insieme *)&tw2;
	iterator it2 = t2.begin();
	while(it2 != t2.end()) {
		it = find(*it2);
		if(it != end()) 
			erase(it);
		it2++;
	}
	return *this;
}

template <class Key, class Cmp>
insieme<Key,Cmp> & insieme<Key,Cmp>::operator += (const insieme & tw2)
{
	// aggiunge ogni elemento di t2 in t1 se non c'e' ancora
	iterator it;
	insieme & t2 = *(insieme *)&tw2;
	iterator it2 = t2.begin();
	while(it2 != t2.end()) {
		it = find(*it2);
		if(it == end()) 
			insert(*it2);
		it2++;
	}
	return *this;
}
	

template <class Key, class Cmp>
insieme<Key,Cmp> & insieme<Key,Cmp>::operator *= (const insieme & tw2)
{
	// rimangono in t1 solo gli elementi in comune con t2
	// remove elements not in t2
	iterator it = begin();
	insieme & t2 = *(insieme *)&tw2;
	iterator it2;
	while(it != end()) {		
		it2 = t2.find(*it);
		if(it2 == end()) 
			erase(it++);
		else
			it++;
	}

	return *this;
}
	
template <class Key, class Cmp>
insieme<Key,Cmp>  insieme<Key,Cmp>::operator * (const insieme & tw2) const
{
	insieme tmp(*this);
	insieme & t2 = *(insieme *)&tw2;
	tmp *= t2;
	return tmp;
}

template <class Key, class Cmp>
insieme<Key,Cmp>  insieme<Key,Cmp>::operator - (const insieme & t2)const
{
	insieme tmp(*this);
	tmp -= t2;
	return tmp;
}

template <class Key, class Cmp>
insieme<Key,Cmp>  insieme<Key,Cmp>::operator + (const insieme & t2)const
{
	insieme tmp(*this);
	tmp += t2;
	return tmp;
}

// confronti tra set
template <class Key, class Cmp>
bool operator== (insieme<Key,Cmp> & s1, insieme<Key,Cmp> & s2)
{ return s1.size() == s2.size() && equal(s1.begin(), s1.end(), s2.begin(), s2.end(), Cmp());}

template <class Key, class Cmp>
bool operator!= (const insieme<Key,Cmp> & s1, const insieme<Key,Cmp> & s2)
{ return !(s1 == s2);}

template <class Key, class Cmp>
bool operator < (const insieme<Key,Cmp> & s1, const insieme<Key,Cmp> & s2)
{ return lexicomp(s1, s2, Cmp());}

template <class Key, class Cmp>
bool operator <= (insieme<Key,Cmp> & s1, insieme<Key,Cmp> & s2)
{ return lexicomp(s1.begin(), s1.end(), s2.begin(),s2.end(), Cmp()) || (s1 == s2);}

template <class Key, class Cmp>
bool operator > (const insieme<Key,Cmp> & s1, const insieme<Key,Cmp> & s2)
{ return !(s1 <= s2);}

template <class Key, class Cmp>
bool operator >= (const insieme<Key,Cmp> & s1, const insieme<Key,Cmp> & s2)
{ return !(s1 < s2);}

#endif