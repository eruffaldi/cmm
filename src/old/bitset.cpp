#include <iostream>
#include "bitset.h"
 

void BitSet::init(int size)
{
	// dimensiona a size
	if(size > WORD_BITCOUNT)
		pData = new unsigned[size / WORD_BITCOUNT];
	mSize = size;	
}

inline unsigned & BitSet::at(int pos) const
{
	if(size > WORD_BITCOUNT)
		return  *(unsigned*)&pData[pos / WORD_BITCOUNT];
	else
		return aItem;
}

inline unisigned BitSet::mask(int pos) const
{
	return ~(~0 << (pos % WORD_BITCOUNT));
}

BitSet::BitSet(const char*s)
{
	const char* cp = s;
	for(int n = 0; *cp != 0 && (*cp == '1' || *cp == '0'); n++)
		cp++;
	init(n);

	// i bit piu' sig. sono ad sx
	for(; n > 0; )
		set(--n, (*s++ - '0') != 0); 			
}

BitSet::BitSet(unsigned l)
{
	init(BITCOUNT);
	iData = l;	// simply!
}

BitSet::BitSet()
{
	init(0);
}


BitSet::~BitSet()
{ if(mSize > BITCOUNT) delete[] pData;}
	
BitSet::reference BitSet::operator[] (int pos)
{
	return reference(at(pos), mask(pos));
}

BitSet::constreference BitSet::operator[] (int pos) const
{ return constreference(at(pos), mask(pos)); }

// sembra strano ma ammetto solo di confrontare fra dimensioni uguali
BitSet& BitSet::operator&= (const BitSet&r)
{
	if(r.mSize != mSize) return *this;

	if(mSize > BITCOUNT) {
		for(int i = 0; i < mSize ; i++)
			pData[i] &= r.pData[i];
	}
	else 
		aItem &= r.aItem;
	return *this;	
}
	
BitSet& BitSet::operator|= (const BitSet&r)
{
	if(r.mSize != mSize) return *this;

	if(mSize > WORD_BITCOUNT) {
		for(int i = 0; i < mSize ; i++)
			pData[i] |= r.pData[i];
	}
	else 
		aItem |= r.aItem;
	return *this;	
}

BitSet BitSet::operator| (const BitSet&r) const
{
	BitSet bs = *this;
	bs |= r;
	return bs;
}

BitSet& operator^= (const BitSet&r);
{
	if(r.mSize != mSize) return *this;

	if(mSize > WORD_BITCOUNT) {
		for(int i = 0; i < mSize ; i++)
			pData[i] ^= r.pData[i];
	}
	else 
		aItem ^= r.aItem;
	return *this;	
}

// questo e' non banale!
BitSet& BitSet::operator<<= (int n)
{
	if(mSize > WORD_BITCOUNT) {
		aItem <<= n;
	}
	else if(n >= mSize) {
		for(int i = 0; i < mSize; i++)
			pData[i] = 0;
	}	                 	
	else {
		int nb = n / WORD_BITCOUNT;
		int nbit = n % WORD_BITCOUNT;
		unsigned mask = ~(~0 << nbit);
		for(int i = mSize-1; i > nb; i--)
			pData[i] = pData[i-nb];
		for(; i >= 0;i-- )
			data[i] = 0;
		if(nbit != 0) {
			unsigned carry = 0;
			for(i = nb; i < mSize; i++) {
				unsigned tmp = pData[i] >> (WORD_BITCOUNT-n);
				pData[i] = pData[i] << nbit | carry;
				carry = tmp & mask;
			}				
		}
	}
	return *this;
}

// uguale al precedente
BitSet& BitSet::operator>>= (int n)
{
	if(mSize > WORD_BITCOUNT) {
		aItem >>= n;
	}
	else if(n >= mSize) {
		for(int i = 0; i < mSize; i++)
			pData[i] = 0;
	}	                 	
	else {
		int nb = n / WORD_BITCOUNT;
		int nbit = n % WORD_BITCOUNT;
		unsigned mask = ~(~0 << nbit);
		for(int i = 0; i < mSize-nb; i++)
			pData[i] = pData[i+nb];
		for(; i < mSize;i++ )
			data[i] = 0;
		if(nbit != 0) {
			unsigned carry = 0;
			for(i = mSize-nb-1; i >= 0; i--) {
				unsigned tmp = pData[i] & mask;
				pData[i] = pData[i] >> nbit | carry;
				carry = tmp;
			}				
		}
	}
	return *this;
}

void BitSet::set()
{
	if(mSize <= WORD_BITCOUNT)
		aItem = ~0;
	else {
		for(int i = 0; i < mSize; i++)
			pData[i] = ~0;
	}
}

void BitSet::set(int pos)
{
	at(pos) |= mask(pos);
}

void BitSet::reset()
{
	if(mSize <= WORD_BITCOUNT)
		aItem = 0;
	else {
		for(int i = 0; i < mSize; i++)
			pData[i] = 0;
	}
}

void BitSet::reset(int pos)
{
	at(pos) &= mask(pos);
}

void BitSet::flip()
{
	if(mSize <= WORD_BITCOUNT)
		aItem = ~aItem;
	else {
		for(int i = 0; i < mSize; i++)
			pData[i] = ~pData[i];
	}
}
	
void BitSet::flip(int pos)
{
	unsigned &r = at(pos);
	unsigned m = mask(pos);
	r = (r& ~m) | ((~r) & m);
}

BitSet BitSet::operator~() const
{
	BitSet spreco(*this);
	spreco.flip();
	return spreco;
}

BitSet BitSet::operator << (int n) const
{
	BitSet b(*this);	
	b <<= n;
	return b;
}

BitSet BitSet::operator >> (int n) const
{
	BitSet b(*this);
	b >>= n;
	return b;
}

unsigned BitSet::to_uint() const
{
	if(mSize > WORD_BITCOUNT) {
		BitCount b(*this);
		b >>= WORD_BITCOUNT;
		return b.pData[0];
	}
	else
		return aItem;
}

char * BitSet::to_string() const
{
	char *buf = new char[mSize];
	char* cp = buf;
	if(!buf) return 0;
	for(int i = mSize-1; i >= 0; i--)
		*cp++ = get(i) ? '1' : '0';
	return buf;
}


bool BitSet::operator== (const BitSet& r) const
{
	if(r.mSize != mSize) return 0;
	if(mSize > WORD_BITCOUNT ) {
		return aItem == r.aItem;	
	}
	else {
		for(int i = 0; i < mSize; i++)	
			if(pData[i] != r.pData[i]) return 0;
		return 1;
	}
}

bool BitSet::operator!= (const BitSet& r) const
{ return !(r == *this);}
	
bool BitSet::test(int pos) const
{ return at(pos) & mask(pos); }

bool BitSet::any() const
{ if(mSize <= WORD_BITCOUNT) return aItem != 0; 
  else {
	for(int i = 0 ; i < mSize; i++)
		if(pData[i] != 0) return 1;
	return 0;
  }
}

bool BitSet::all() const
{
	if(mSize <= WORD_BITCOUNT) 
		return !(~(aItem & mask(mSize)));
	else {
		for(int i = 0; i < mSize-1; i++) 
			if(pData[i] != ~0) return 0;
		return !(~(pItem[i] & mask(mSize)) ); 
}
	


