// Nome: AutoPtr
// Progetto: CPP/Tools
// Descrizione: Puntatori dinamici con dynamic type checking
// 
// Autore: Ruffaldi Emanuele
//
// auto_ptr
// Questo tipo di puntatori sono molto simili a quelli messi a 
// dispozione dalle STL. Sono puntatori sicuri perche' distruggono
// l'oggetto ad essi associato.
//
// dyn_ptr
// Puntatore polimorfico che controlla usando RTTI il tipo dei
// dati inseriti, questo consente di fare upcast e downcast
// in modo sicuro.
// Richiede supporto RTTI e template members
#ifndef AUTOPTR_H
#define AUTOPTR_H

#pragma warning(disable: 4290)
typedef int bad_dynamic_cast;

template<class T> class auto_ptr {
public:    
	typedef T element_type;
    explicit auto_ptr(T *p = 0) throw() : _p(p) , owned(p != 0) {};

    auto_ptr(const auto_ptr<T>& rhs) throw() 
	{
		rhs.owned = owned;
		owned = false;
		rhs._p = _p;
	}
    
	auto_ptr<T>& operator=(auto_ptr<T>& rhs) throw()
	{
		if(rhs != * this) {
			if(owned) delete _p;
			owned= rhs.owned;
			rhs.owned = false;
			_p = rhs._p;
		}
		return *this;
	}

	~auto_ptr() {if(owned) delete _p;}

    T& operator*() const throw() {return *p;}
	T *operator->() const throw() {return get();}
    T *get() const throw()   {return _p;}
	T *release() const {owned = false; return _p;}
protected:
	T * _p;
	bool owned;
};

template<class T>    class dyn_ptr {
public:    
	typedef T element_type;

	dyn_ptr() throw() : _p(0) {}

	// costruttore con il puntatore ad un tipo qualsiasi
	template <class B>
    dyn_ptr(B *p) throw(bad_dynamic_cast) : _p(dynamic_cast<T*>(p)) 
		 {if(!p) throw bad_dynamic_cast();};

	// costruttore di copia polimorfico
	template <class W>
		dyn_ptr(const dyn_ptr<W> & rhs) : _p(dynamic_cast<T*>(rhs.get()))  {
		if(rhs.get() && !_p) throw bad_dynamic_cast();
	}

    // assegnamento polimorfico
	template <class W>
	dyn_ptr<T>& operator= (dyn_ptr<W>& rhs) throw()
	{	if(rhs != *this) _p = dynamic_cast<T*>(rhs.get()); return *this;  }

	// assegmanento polimorfico da stesso tipo
	template <class W>
	dyn_ptr<T>& operator= (W * p) throw()
	{	_p = dynamic_cast<T*>(p); return *this;  }

	operator T* () {return _p;}

	template <class W>
	operator dyn_ptr<W> () throw()
	{	return dyn_ptr<W>(_p);  }
	
	~dyn_ptr() {}

    T& operator*() const throw() {return *_p;}
	T *operator->() const throw() {return _p;}
    T *get() const throw()   {return _p;}	
protected:
	T * _p;
};

#endif