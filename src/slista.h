// Implementazione di una lista Singolarmente Linkata
// Autore: Ruffaldi Emanuele
// Base:
// insert(posBefore, value)
// insert(posBefore, duplicates, value)
// push_front, push_back, erase(pos), erase(first, last), front(), back()
// begin(), end(), clear()
// 
// Requisiti:
// 1) inserisci ordinato: usa child class
// 2) ordina: sort()
// * 3) estrai per valore: isPresent(value)
// * 4) conta: count()
// * 5) conta duplicati di un valore: count(value)
// * 6) inverti: revert()
// 7) unisci: insert(posizione, primo, secondo)
// 8) unisci ordinato: merge(lista)
// 9) vuoto: empty()
// 
// La lista e' implementata in modo circolare. Con un unico puntatore
// la coda controllo anche la testa.
#ifndef SLISTA_H
#define SLISTA_H

template <class T>
class slista {
protected:
    class node {
    public:
        T     m_value;
        node* m_next;
    public:
        node(const T& v, node*pnext) : m_value(v), m_next(pnext) {}
		
    };
	inline static node * & Next(node * p) {return p->m_next;}
    node* m_tail, * m_head;			// un poco ingombrante!
    
public:
    typedef T& reference;
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef const T& const_reference;
    typedef T value_type;

	friend class iterator;

    class iterator {        
    protected:
        node* m_item;
    public:
        iterator(node*it): m_item(it) {}
		iterator() : m_item(0) {}
        reference operator* () { return m_item->m_value;}
        bool operator==(const iterator& it) const { return m_item == it.m_item; }
        bool operator!=(const iterator& it) const { return m_item != it.m_item; }
		iterator & operator++() { m_item = m_item->m_next; return *this;}
		iterator operator++(int) 	{ iterator it(m_item); ++(*this); return it; }
		
		iterator operator+(int i) const
		{
			iterator it(m_item);
			while(i--) 
				it.m_item = it.m_item->m_next;			
			return it;
		}

        // distanza intesa come passi da compiere per passare da
        // secondo - primo
        // -1 se non sono sulla stessa lista oppure nell'ordine errato
        int operator-(const iterator& it) {
            int n = 0;
            node* item = it.m_item;
            while(item != m_item) {
                item = item->m_next;
                n++;
            }
            return n;
        }
		node* _node() const {return m_item;}
    };

    slista() { m_head = m_tail = 0;}

    void push_back(const T& v) {insert(end(), v); }
    void push_front(const T& v) { insert(begin(), v);}
    void pop_front() { erase(begin());}

    reference front() { return *begin(); }
    reference back()  { return m_tail->m_value;}
    bool empty() { return m_head == 0;}

    iterator begin() {return iterator(m_head);}
    iterator end() {return iterator(0);}
    int count()  { return end() - begin(); }
    int count(const_reference v) ;
    bool isPresent(const_reference v) const; 

    void revert();    
	// aggiunge v prima di pos
    void insert(iterator pos, const_reference v ) { insertElement(pos._node(), v);}

	// aggiunge n copie di v prima di pos
    void insert(iterator pos, int n, const_reference v);
    // aggiunge elementi da [first:second[ prima di pos
    void insert(iterator pos, iterator first, iterator second);

    // unisce gli elementi di due liste ordinate
    // sono due loop
    void merge(slista& li);

    // elimina l'elemento in pos
    void erase(iterator pos);

    void clear() { erase(begin(), end());}

    // cancella [first:last[
    void erase(iterator first, iterator last);

    // splice: move all elements from x before pos without copying
    void splice(iterator pos, slista&x) { splice(pos, x, x.begin(), x.end()); }

    // splice: move *p from x before pos without copying
    void splice(iterator pos, slista&x, iterator p) { splice(pos, x, p, p+1); }

	// splice: move [first:last[ before pos without copying
    void splice(iterator pos, slista&x, iterator first, iterator last);        
    void swap(slista& x);
    void sort();

    iterator binary_search(const_reference v)
    { return binary_search(begin(), end(), v); }

    // Ricerca binaria da [first:last[ su una lista ordinata
    // E' INDIPENDENTE DALLA STRUTTURA DATI LISTA
    // - prendo l'elemento mediano
    // - se il dato e' minore uguale al mediano ritorno, se e' minore
    //   lo applico alla prima meta', se e' maggiore alla seconda.
    // Nel caso della lista la cosa e' un po' inefficiente, ma la ricerca
    // binaria e' cosi'. Per migliorare la situazione quando andiamo in
    // avanti salviamo la vecchia posizione centrale in modo tale che la
    // volta successiva non dobbiamo ripartire da 0.
    // Ritorna: un iteratore tra first e last escluso, se non trova
    // ritorna last
    iterator binary_search(iterator first, iterator last, const_reference v);    
protected:
    // inserisce un elemento prima di item e ritorna l'elemento aggiunto
    node* insertElement(node* item, const_reference v, node*prev = 0);    
	node* itemPrevious(iterator pos, node* from = 0);
	void sgancia(iterator primo, iterator ultimo);
	void aggancia(iterator precedente, iterator primo, iterator ultimo);
	node * mergelist(node * p1, node * p2);

};

template <class T>
int slista<T>::count(const T& v) 
{
	iterator it = begin();
	int n;
    for( n = 0; it != end(); it++)
		if(*it == v)
	     	n++;
    return n;    
}

template <class T>
bool slista<T>::isPresent(const T& v) const 
{
        iterator it = begin();
        for(; it != end(); it++)
            if(*it == v)
                return true;
        return false;        

}

template <class T>
void slista<T>::revert()
 {
        node * first, *second;
        if(m_head == m_tail)    // at least two elements
            return;
        first = m_head;
        second = m_head->m_next;
        m_tail = first;
        first->m_next = 0;
        do {
            node* third = second->m_next;
            second->m_next = first;
            first = second;
            second = third;
        } while(second != 0);
        m_head = first;
  }



template <class T>
void slista<T>::insert(iterator pos, int n, const_reference v) 
{
    if(n <= 0) return;
    node *item = insertElement(pos.m_item, v);

    // dopo il primo accodiamo gli altri
    while(--n) {
        node*newItem = new node(v, item->m_next);
        item->m_next = newItem;
        if(item == m_tail)
            m_tail = item;
            item = newItem;
    }        
}

template <class T>
void slista<T>::insert(iterator pos, iterator first, iterator second) 
{
    // first insert, checks for add to end of list and null list
    if(first == second)
        return;
    node * item = insertElement(pos.m_item, *first);
    first++;
    while(first != second) {
        node* newItem = new node(*first, item->m_next);
        if(item == m_tail)
            m_tail = newItem;
        item->m_next = newItem;            
        item = newItem;
        first++;
    }
}

// aggancia a prima di precedente la serie [primo..ultimo]
template <class T>
void slista<T>::aggancia(iterator precedente, iterator primo, iterator ultimo)
{
	node * prima_ultimo = itemPrevious(ultimo._node(), primo._node());
	if(empty()) {
		m_head = primo._node();
		m_tail = prima_ultimo;
		Next(m_tail) = 0;
		return;
	}
	node * prec = precedente._node();
	node * oldlast = itemPrevious(prec);		
	Next(prima_ultimo) = prec;	
	if(oldlast == 0) 
		m_head = primo._node();
	else
		Next(oldlast) = primo._node();	
}

// stacca [primo, ultimo)
template <class T>
void slista<T>::sgancia(iterator primo, iterator ultimo)
{
	if(primo == begin() && ultimo == end()) {
		m_head = m_tail = 0;
		return;
	}	
	
	if(primo != begin()) {
		node * prev = itemPrevious(primo._node());
		Next(prev) = ultimo._node();		
		if(ultimo == end()) 
			m_tail = prev;
	}
	else
		m_head = ultimo._node();	
}

/*
// unisce gli elementi di due liste ordinate
// sono due loop
template <class T>
void slista<T>::merge(slista& li) {
    iterator itDest = begin();
    iterator itSrc = li.begin();
    node* prev = 0;        // hint per l'inserimento
    while(itSrc != itDest) {
        const_reference v = *itSrc;
        // ricerca una posizione
        while(itDest != end() && *itDest < v) {
            prev = itDest._node();
            itDest++;
        }
        prev = insertElement(itDest._node(), v, prev);
        itDest = iterator(prev);
        itSrc++;
    }
}
*/

template <class T>
typename slista<T>::node * slista<T>::mergelist(node * p1, node * p2)
{
	if(!p1) return p2;
	if(!p2) return p1;
	if(p1->m_value < p2->m_value) {
		Next(p1) = mergelist(Next(p1), p2);
		return p1;
	}
	else {
		Next(p2) = mergelist(p1, Next(p2));
		return p2;
	}
}

template <class T>
void slista<T>::merge(slista& li) {
	m_head = mergelist(m_head, li.m_head);
	if(!m_head)
		m_tail = 0;
	else
		m_tail = itemPrevious(0);
	li.m_head = li.m_tail = 0;
}

// elimina l'elemento in pos
template <class T>
void slista<T>::erase(iterator pos) {
    node * prev = m_head;
    if(pos == end()) return;
    // se non e' il primo cerchiamo l'elemento precedente
    if(pos != begin()) {
        while(prev && Next(prev) != pos._node())
            prev = Next(prev);            
        Next(prev) = Next(pos._node());
    }
    else {
        m_head = Next(pos._node());
    }

    // se l'elemento era l'ultimo lo facciamo puntare al precedente                
    if(pos._node() == m_tail)
        m_tail = prev;

    delete pos._node();
}

// cancella [first:last[
template <class T>
void slista<T>::erase(iterator first, iterator last) {
    node * prev = m_head;
    if(first != begin()) {
        while(prev && prev->m_next != first._node())
            prev = prev->m_next;            
    }
    else {
        m_head = last._node();
    }
    prev->m_next = last._node();
    // aggiusta la coda se tagliamo fino all'ultimo
    if(prev->m_next == 0)
        m_tail = prev;

    while(first != last) {
        node* toDel = first._node();
        first++;
        delete toDel;
    }
}

template <class T>
// inserisce un elemento prima di item e ritorna l'elemento aggiunto
typename slista<T>::node* slista<T>::insertElement(node* item, const_reference v, node*prev)
{
    node *newItem = new node(v, item);
    if(item == m_head)  {
        m_head = newItem;
        if(m_tail == 0)
            m_tail = m_head;
    }
    else if(item == 0)  {
        Next(m_tail) = newItem;
        m_tail = newItem;
    }
    else {
        node* pItem = m_head;
        if(prev != 0)
            pItem = prev;
        else {
            while(pItem && pItem->m_next != item)
                pItem = pItem->m_next;
            // ora ci troviamo giusto prima dove inserire
        }
        pItem->m_next = newItem;
    }
    return newItem;
}


// splice: move [first:last[ before pos without copying
template <class T>
void slista<T>::splice(iterator pos, slista&x, iterator first, iterator last) 
{
    if(first == last) return;
    x.sgancia(first, last);
	aggancia(pos, first, last);
}

template <class T>
void slista<T>::swap(slista& x) {
    node *tmp;
    tmp = x.m_head;
    x.m_head = m_head;
    m_head = tmp;
    tmp = x.m_tail;
    x.m_tail = m_tail;
    m_tail = tmp;
}

// 1) ordina gli elementi distribuendoli fra due liste
template <class T>
void slista<T>::sort() {
    slista<T> aux1;
    slista<T> aux2;

    while(!empty()) {
        aux1.splice(aux1.begin(), *this, begin());
        aux1.merge(aux2);
        aux1.swap(aux2);
    }
    merge(aux1);
    merge(aux2);
}


// Ricerca binaria da [first:last[ su una lista ordinata
// E' INDIPENDENTE DALLA STRUTTURA DATI LISTA
// - prendo l'elemento mediano
// - se il dato e' minore uguale al mediano ritorno, se e' minore
//   lo applico alla prima meta', se e' maggiore alla seconda.
// Nel caso della lista la cosa e' un po' inefficiente, ma la ricerca
// binaria e' cosi'. Per migliorare la situazione quando andiamo in
// avanti salviamo la vecchia posizione centrale in modo tale che la
// volta successiva non dobbiamo ripartire da 0.
// Ritorna: un iteratore tra first e last escluso, se non trova
// ritorna last
template <class T>
typename slista<T>::iterator slista<T>::binary_search(iterator first, iterator last, const_reference v)
{
    int passo = (last-first)/2;     // distanza fra i due iteratori
    iterator pos = first + passo;
    iterator svpos = first;
    do {
        passo /= 2;
        if(!passo)
            return last;
        if(*pos == v)
            return pos;
        else if(*pos < v) {
            pos = svpos + passo;
        }                
        else {
            svpos = pos;    // salvo se dovro' tornare indietro
            // pos += passo;   // nuova posizione centrale
        }
    } while(1);
}

template <class T>
typename slista<T>::node* slista<T>::itemPrevious(iterator pos, node* from ) 
{
    if(from == 0)
        from = m_head;	
	
    while(from->m_next != pos._node()) 
        from = Next(from);	
    return from;
}


#define FOR_EACH(item , w, itt, t)      for(itt it = w .begin(); it != w .end(); ++it) { t item = *it;
#define END_EACH()       }

#endif