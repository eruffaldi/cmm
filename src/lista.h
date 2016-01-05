#ifndef LISTA_H

// Lista doppiamente linkata tipo STL
// di Ruffaldi Emanuele 1998
template <class T>
class lista {
	struct nodo {
		nodo * next;
		nodo * prev;
		T		dato;
		nodo(const T& t) : dato(t) {}
	};
public:
	class iterator;
	friend class iterator;

	class iterator {
		nodo * ptr;	
	public:
		iterator(nodo * p): ptr(p) {}
		iterator succ() const { return iterator(ptr->next);}
		iterator prev() const { return iterator(ptr->prev);}
		iterator & operator++() { ptr = ptr->next; return *this;}
		iterator & operator--() { ptr = ptr->prev; return *this;}
		iterator operator++(int) {
			iterator tmp = * this;
			++*this;
			return tmp;
			}
		iterator operator--(int) {
			iterator tmp = *this;
			--*this;
			return tmp;
			}
		T & operator* () {return ptr->dato;}
		T * operator->() {return &**this;}
		int operator == (const iterator &r) const {return ptr ==r.ptr; }
		int operator != (const iterator& r) const {return ptr != r.ptr;}
		nodo * getNode() const {return ptr;}
		int isLast() const {return ptr == 0 || ptr->next == 0;}
	};

	class const_iterator;
	friend class const_iterator;
	class const_iterator: iterator {
	public:
		const_iterator(nodo *p) : ptr(p) {}
		const_iterator succ() const { return iterator(ptr->next);}
		const_iterator prev() const { return iterator(ptr->prev);}
		const_iterator & operator++() { ptr = ptr->next; return *this;}
		const_iterator & operator--() { ptr = ptr->prev; return *this;}
		const_iterator operator++(int) {
			iterator tmp = * this;
			++*this;
			return tmp;
			}
		const_iterator operator--(int) {
			iterator tmp = *this;
			--*this;
			return tmp;
			}
		T & operator* () const {return ptr->dato;}
		T * operator->() const {return &**this;}
		int operator == (const const_iterator &r) const {return ptr ==r.ptr; }
		int operator != (const const_iterator& r) const {return ptr != r.ptr;}
		
	};

	lista();
	void elimina(iterator);
	void accoda(const T &);
        iterator begin() { return iterator(testa);}
        iterator end() {return iterator(0);}
        const_iterator begin() const {return const_iterator(testa);}
        const_iterator end() const {return const_iterator(0);};
	void inserisci(const T&, iterator);
protected:
	node * testa;	
	node * coda;
};


lista<T>::lista()
{
        begin = end = 0;
}


void lista<T>::elimina(iterator p)
{
	nodo * np = p.getNode();
	if(np == testa)
		testa = np->next;
	else
		np->prev->next = np->next;
	if(np == coda) 
		np->next->prev = np->prev;
	else
		coda = np->prev;
	delete np;
}

void lista<T>::accoda(const T &p);
{
	insert(p, iterator(0));
}

void lista<T>::inserisci(const T&t, iterator p)	
{
	nodo * np = p.getNode();
	nodo * nuovo = new nodo(t);
	if(coda == 0) {
		testa = coda = nuovo;
		nuovo->next = nuovo->prev = 0;
		return;
	}

	// accodamento
	if(np == 0) {
		coda->next = nuovo;
		nuovo->prev = coda;
		nuovo->next = 0;
		coda = nuovo;
	}
	else {
		nuovo->next = np;
		nuovo->prev = np->prev;			
		if(np == testa) 
			nuovo = testa
		else 
			np->prev->next = nuovo;
		np->prev = nuovo;
	}
}


#endif
