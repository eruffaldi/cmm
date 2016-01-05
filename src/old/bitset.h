#ifndef BITSET_H
#define BITSET_H

//typedef int bool;
#define  WORD_BITCOUNT (sizeof(unsigned)*8)


class BitSet {

	class reference {
		friend class BitSet;
		unsigned & ref;
		unsigned  mask;
	public:
		reference(unsigned &r , unsigned m): ref(r), mask(m) {}
    	bool operator~() { return !(ref & mask);}
		operator bool() const { return !(!(ref&mask));}
		void flip() { ref = (ref & ~mask) | ((~ref) & mask);}
		reference& operator= (bool b) { ref = (ref & ~mask) | (b? mask:0); 
			return *this;}
		reference& operator= (const reference&r) { *this = bool(ref); return
*this;}
		
	};

	class constreference {
		friend class BitSet;
		const unsigned & ref;
		unsigned mask;
	public:
		constreference(const unsigned &r, unsigned m): ref(r), mask(m) {}
		operator bool() const { return !(!(ref&mask));}	
	};
	
	
	int mSize;
	union {
		unsigned *pData;
		unsigned aItem;		// only one mSize <= BITCOUNT, see at
	};
	void init(int size);
	unsigned & at(int pos) const;
	unsigned mask(int pos) const;
public:
    BitSet(const char*);
	BitSet(unsigned  l);
	BitSet();	// nessun bit
	~BitSet();
	
	reference operator[] (int pos);
	constreference operator[] (int pos) const;
	BitSet& operator&= (const BitSet&r);
	BitSet& operator|= (const BitSet&r);
	BitSet operator| (const BitSet&r) const ;	BitSet& operator^= (const BitSet&r);
	BitSet& operator<<= (int n);
	BitSet& operator>>= (int n);
	void set();
	void set(int pos);
	void reset();
	void reset(int pos);
	void flip();
	void flip(int pos);
	BitSet operator~() const;
	BitSet operator << (int n) const;
	BitSet operator >> (int n) const;
	unsigned  to_uint() const;
	char * to_string() const;
	bool operator== (const BitSet& r) const;
	bool operator!= (const BitSet& r) const;
	bool test(int pos) const;
	bool any() const ; 	// almeno uno e' settato
	bool none() const {return !any();} // nessuno e' settato
	bool all() const;	// tutti settati

};
#endif
