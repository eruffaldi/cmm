// Nome: Mappa
// Progetto: CPP/Tools
// Autore: Ruffaldi Emanuele
// Descrizione: mappa associativa fra tipi basata su albero
// binario. Si tratta di un adapter del tipo binary tree
#ifndef MAPPA_H
#define MAPPA_H

#include "btree.h"

template <class Key, class T, class Cmp = less<Key> >
class mappa 
{
public:
	typedef pair<Key,T> value_type;
	typedef key_compare<Key, T, Cmp> compare_type;
	typedef binTree< value_type, compare_type > tree;
	typedef typename tree::iterator iterator;	

protected:
	tree _tree;
public:
	

	void erase(const Key& k);
	void erase(iterator it1, iterator it2) { _tree.erase(it1, it2);}
	void erase(iterator it1) { _tree.erase(it1);}
	iterator find(const Key&k);
	T & operator[] (const Key&k);
	void clear() {return _tree.clear();}
	int size() const {return _tree.size();}
	
	pair<iterator, bool> insert(const value_type& x) 
		{return _tree.insert(make_pair(x, T()));}	
	iterator begin() {return _tree.begin();}
	iterator root() {return _tree.root();}
	iterator end() {return _tree.end();}


};

template <class Key, class T, class Cmp >
void mappa<Key,T,Cmp>::erase(const Key& k)
{
	_tree.erase(make_pair(Key(k), T()));
}

template <class Key, class T, class Cmp >
typename mappa<Key,T,Cmp>::iterator mappa<Key,T,Cmp>::find(const Key&k)
{
	return _tree.find(make_pair(k, T()));
}

template <class Key, class T, class Cmp >
T & mappa<Key,T,Cmp>::operator[] (const Key&k)
{
	return (*(_tree.insert(make_pair(k,T())).first)).second;
}

#endif