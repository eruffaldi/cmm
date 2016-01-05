// Nome: dvector
// Autore: Ruffaldi Emanuele
// Descrizione: vettore a dimensione variabile
#ifndef DVECTOR_H
#define DVECTOR_H

template <class T> 
class dvector {
public:
	typedef int iterator;
protected:
	T * m_data;
	int m_dim;
	int m_cap;
public:
	dvector<T>(const dvector&r): m_cap(0)
		{
			grow(r.m_dim);
			m_dim = r.m_dim;
		    for(int i = 0; i < m_dim; i++) 
				m_data[i] = r.m_data[i];		
		}
	dvector<T>() :m_cap(0), m_dim(0), m_data(0) {}
	dvector<T>(int n , T v = T()) :m_cap(0)
		{
			grow(n);
			m_dim = n;
			for(int i = 0; i < m_dim; i++)
				m_data[i] = v;
		}

	dvector<T>(const dvector&r, iterator inizio, iterator fine):m_cap(0)
		{
		   int n = fine-inizio;
		   if(r.size() < n) {
				m_cap = m_dim = 0;
				return;
			}	
			else
				grow(n);
			m_dim = n;
			for(int i = 0; i < n ;i++)
				m_data[i] = r.m_data[inizio+i];
	 	}

	~dvector<T>() 
		{
			delete[] m_data;
		}
	
	T& operator [](int pos) const
		{ return m_data[pos]; }

	// rimozione                         
	void cut(iterator inizio, iterator fine)
		{
			if(!m_dim) return;
			// taglia gli elementi
			for (iterator p = inizio; p < end(); p++)
				m_data[p] = m_data[p+fine];
			m_dim -= fine-inizio;
		}
	void remove(iterator singolo)
		{
			if(!m_dim) return;
			for(iterator p = singolo+1; p < end(); p++)
				m_data[p-1] = m_data[p];
			m_dim--;
		}

	// split
	void split(iterator posizione, dvector&v)
		{
			v.insert(*this, posizione);
			if(posizione != begin())
				cut(begin(), posizione-1);
		}

	// inserimento
	iterator insert(T v, iterator before)
		{
			if(m_cap == m_dim) 
				grow(1);
			m_dim++;
			// muovi gli elementi
			for(iterator p = before+1; p < end()-1; p++)
				m_data[p] = m_data[p-1];
			m_data[before] = v;
			return before;           
		}
	
	iterator insert(const dvector&r, iterator inizio)
		{
		    if(m_cap <= size()+r.size())
				grow(r.size());

			// muovi gli elementi da inizio a inizio+r.size()
			for(iterator p = inizio+1; p < end(); p++)
				m_data[p] = m_data[p-1];
			m_dim += r.size();     
			for(p = 0; p < r.end(); p++)
				m_data[p+inizio] = r.m_data[p]; 
			return inizio;          
		}	
	void pushback(T v)
		{
	  		if(m_cap == m_dim)
				grow(1);
			m_data[m_dim++] = v;		
		}

	dvector& operator= (const dvector&v)
	{
		delete[] m_data;
		m_cap = m_dim = v.m_dim;
		m_data = new T[m_dim];
		for(int i = 0; i < m_dim; i++)
			m_data[i] = i;
		return *this;
	}

	// informazioni
	iterator begin() const {return 0;}
	iterator end() const {return m_dim;}
    int size() const {return m_dim;};
	int count() const {return m_dim;};
	int capacity() const {return m_cap;}
	
protected:
	void grow(int n) {
		if(m_cap == 0) {
			m_data = new T[n];
			m_cap = n;
			m_dim = 0;
		} 
		else {
			// alloca nuova memoria e ricopia il tutto 
			m_cap = m_cap*2 < n ? n: m_cap*2;
			T* newdata = new T[m_cap];
			for(int i = 0; i < m_dim; i++)
				newdata[i] = m_data[i];
			delete[] m_data;
			m_data = newdata;
		}
	} 
};

#endif