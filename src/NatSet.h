// Nome: natset
// Progetto: CPP/Tools
// Autore: Ruffaldi Emanuele
// Descrizione: small set di naturali, il numero massimo dipende
// dalla dimensione in bit di unsigned long
#ifndef NATSET_H
#define NATSET_H

// Release 2: compatibilita' con BitSet
// per le chiamate.
class NatSet {
    unsigned long m_set;
	typedef unsigned long TGT ;
public:
    NatSet() { m_set = 0;}
    NatSet(unsigned int p)  { m_set = 1 << p;}
    NatSet operator* (const NatSet& r) const {
        NatSet ret;
        ret.m_set = m_set & r.m_set;
        return ret;
    }

    NatSet operator+ (const NatSet& r) const {
        NatSet ret;
        ret.m_set = m_set | r.m_set;
        return ret;
    }

    NatSet operator- (const NatSet& r) const  {
        NatSet ret;
        ret.m_set = m_set & ~r.m_set;
        return ret;
    }

    NatSet& operator += (const NatSet&r)  {
        m_set |= r.m_set;
        return *this;
    }

    NatSet& operator*= (const NatSet&r)  {
        m_set &= r.m_set;
        return *this;
    }

    NatSet& operator-= (const NatSet&r)  {
        m_set &= ~r.m_set;
        return *this;
    }    

    bool operator== (const NatSet&r) const {return m_set == r.m_set;}
    bool operator!= (const NatSet&r) const {return m_set != r.m_set;}
    bool operator>= (const NatSet&r) const {
        return (m_set & r.m_set) == r.m_set;
    }

    bool operator<= (const NatSet&r) const {
        return (m_set & r.m_set) == m_set;
    }

	bool test(int pos) const {return (m_set& (1<<pos)) != 0;}
	bool any() const { return m_set != 0;}
	bool none() const { return m_set == 0;}
	bool all() const { return m_set == ~0;}
    operator bool() const { return m_set != 0;}
    
	unsigned int capacity() const {return sizeof(unsigned long)*8;}
    int size() const {
        int set = m_set;
        int n;
		for(n = 0; set != 0; set >>= 1)
            if(set & 1) n++;
        return n;
    }

	// stl container 
	bool empty() const {return none();}	
	

	// Enumerazione
    void begin(unsigned int&n ) const { n = 0;}
    bool ok(unsigned int n) const {
        unsigned long m = m_set;
		int i;
        for(i = 0; m  ; i++) m>>=1;
        return i!= 0 && int(n) <= i-1 ? true : false; 
    }

    int next(unsigned int &n) const {
        while((m_set & (1 << n)) == 0) n++;
        return n++;
    }

};

inline std::ostream & operator << (std::ostream & o, const NatSet & r)
{
	o << '(';
	unsigned int k;
	r.begin(k);
	while(r.ok(k)) {
		int v = r.next(k);
		o << v << ' ';
	};
	o << ')';
	return o;
}

#endif
                        
