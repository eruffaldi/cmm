// Nome: Binary Tree
// Progetto: CPP/Tools
// Autore: Ruffaldi Emanuele
// Descrizione: albero binario (non autobilanciato)
// usato dalla classe mappa
#ifndef BTREE_H
#define BTREE_H

#include "pair.h"

template <class T, class Cmp = less<T> >
class binTree {
public:

	class iterator;
	friend class iterator;
private:
	struct node;
	typedef node* pnode;

	struct node {
		pnode _L, _R, _P;
		T	  _Value;
		node(const T& v, pnode parent): _Value(v), _L(0), _R(0), _P(parent) {}
	};

	inline static pnode Left(pnode p) {return p->_L;}
	inline static pnode Parent(pnode p) {return p->_P;}
	inline static pnode Right(pnode p) {return p->_R;}
	inline static T& Value(pnode p) {return p->_Value;}
	inline static pnode Leftmost(pnode p) 
	{
		if(!p) return p;
		while(Left(p) != 0)
			p = Left(p);
		return p;
	}

	inline static pnode Rightmost(pnode p) 
	{
		if(!p) return p;
		while(Right(p) != 0)
			p = Right(p);
		return p;
	}

	pnode m_root;
	int   m_count;

	pnode _find(const T&v, int & k, pnode & parent);
	void kill(pnode p);
	
public:
	
	typedef T value_type;
	typedef value_type * pointer;
	typedef binTree<T, Cmp> albero;

	class iterator {
		pnode p;
	public:
		iterator(pnode nodo) : p(nodo) {}
		iterator() :p(0) {}
		T & operator *() const { return Value(p);}
		pointer operator->() const { return &**this;}
		iterator left() const {return iterator(Left(p));}
		iterator right() const {return iterator(Right(p));}
		iterator up() const {return iterator(Up(p));}
		bool operator==(const iterator & it) const {return it.p == p;}
		bool operator!=(const iterator & it) const {return it.p != p;}
		iterator& operator--()
		{
			if(Left(p))
				p = Rightmost(Left(p));
			else {
				pnode tmp = p;
				p = Parent(p);
				while(p != 0) {
					if(Right(p) == tmp)
						break;	// fermati al padre se era nodo destro
					else {
						tmp = p; p = Parent(p);
					}
				};
			}
			return *this;		
		}

		iterator& operator++() {
			if(Right(p))
				p = Leftmost(Right(p));
			else {
				pnode tmp = p;
				p = Parent(p);
				while(p != 0) {
					if(Left(p) == tmp)
						break;	// fermati al padre se era nodo sinistro
					else {
						tmp = p; p = Parent(p);
					}
				};
			}
			return *this;
		}
		iterator operator++(int)
			{   iterator it(p);	++(*this); return it; }
		iterator operator--(int)
			{   iterator it(p);	--(*this); return it; }
		pnode  _node() const {return p;}
	};

	binTree();
	binTree(const binTree&);
	int size() const {return m_count;}
	iterator find(const T&) const;
	std::pair<iterator,bool> insert(const T&);
	void clear();
	void erase(iterator);
	void erase(iterator, iterator);
	void erase(const T&);
	iterator root() const {return iterator(m_root);}
	iterator begin() const {return iterator(Leftmost(m_root));}
	iterator end() const {return iterator(0);}

};

// Implementazione della binTree

template <class T, class Cmp >
binTree<T,Cmp>::binTree()
{ m_count = 0; m_root = 0;}


template <class T, class Cmp >
binTree<T,Cmp>::binTree(const binTree&r)
{
	m_root = 0; m_count = 0;
	iterator it = r.begin();
	while(it != r.end()) {
		insert(*it);
		it++;
	}
}

template <class T, class Cmp >
typename binTree<T,Cmp>::iterator binTree<T,Cmp>::find(const T&v) const
{
	iterator cur = root();
	Cmp compare;
	while(cur != end()) {
		if(compare(*cur, v))
			cur = cur.right();
		else if(compare(v,*cur))
			cur = cur.left();
		else
			break;
	}
	return cur;
}

template <class T, class Cmp >
typename binTree<T,Cmp>::pnode binTree<T,Cmp>::_find(const T&v, int & k, pnode & parent)
{
	pnode tparent = m_root;
	pnode cur = m_root;
	k = 0;
	Cmp compare;
	while(cur) {
		if(compare(Value(cur), v)) {
			tparent = cur;
			k = 1;
			cur = Right(cur);
		}
		else if(compare(v,Value(cur))) {
			tparent = cur;
			k = -1;			
			cur = Left(cur);
		}
		else
			break;
	}
	parent = tparent;
	return cur;
}

template <class T, class Cmp >
void binTree<T,Cmp>::erase(const T& v)
{
	erase(find(v));
}

template <class T, class Cmp >
void binTree<T,Cmp>::erase(iterator it)
{
	if(it == end()) return;
	pnode parent, cur, nuovo;
	cur = it._node();
	parent = Parent(cur);

	if(!Right(cur)) 
		nuovo = Left(cur);
	else if(!Left(cur))
		nuovo = Right(cur);
	else {
		nuovo = Left(cur);
		pnode ps = Leftmost(Right(cur));
		ps->_L = Right(nuovo);
		if(nuovo->_R)
			nuovo->_R->_P = ps;
		nuovo->_R = Right(cur);
		cur->_R->_P = nuovo;
	}

	if(nuovo)
		nuovo->_P = parent;

	if(parent) {
		if(cur == Left(parent))
			parent->_L = nuovo;
		else 
			parent->_R = nuovo;
	}
	else
		m_root = nuovo;
	m_count--;
	delete cur;	
}

template <class T, class Cmp >
void binTree<T,Cmp>::erase(iterator it1, iterator it2)
{
	while(it1 != it2) 
		erase(it1++);	
}

template <class T, class Cmp >
void binTree<T,Cmp>::kill(pnode p)
{

	if(Left(p) != 0)
		kill(Left(p));
	if(Right(p) != 0)
		kill(Right(p));
	delete p;
}

template <class T, class Cmp >
void binTree<T,Cmp>::clear()
{
	if(m_root)
		kill(m_root);
	m_root = 0;
	m_count = 0;
}

template <class T, class Cmp >
std::pair< typename binTree<T,Cmp>::iterator,bool> binTree<T,Cmp>::insert(const T& v)
{
	int k; 
	pnode cur, parent;
	cur = _find(v, k, parent);
	if(cur != 0)
		return make_pair(iterator(cur), true);
	pnode nuovo = new node(v, parent);
	if(!m_root)
		m_root = nuovo;
	else if(k > 0)
		parent->_R = nuovo;
	else if(k < 0)
		parent->_L = nuovo;
	m_count++;
	return make_pair(iterator(nuovo), false);
}
#endif