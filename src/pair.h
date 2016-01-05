// Nome: pair.h
// Progetto: CPP/Tools
// Autore: Ruffaldi Emanuele
// Descrizione: operazioni template utili per le strutture
// dati, molto simili a strutture STL;
#ifndef PAIR_H
#define PAIR_H

#include <string.h>

// concetto di "paio"
template <class F, class S> 
class pair 
{
public:
	F first;
	S second;

	typedef F first_type;
	typedef S second_type;
	pair(const F& fv, const S& sv) : first(fv), second(sv) {}
	pair() {}
};

// costruzione di paia di oggetti
template <class T1, class T2>
pair<T1,T2> make_pair(const T1& t1, const T2 & t2)
{ return pair<T1,T2>(t1, t2);}

template <class T> 
class less {
public:
	typedef T opd1_type;
	typedef T opd2_type;
	typedef bool ret_type;

	bool operator()(const T& v1, const T& v2)
	{return v1 < v2;}
};

template <class T> 
class same {
public:
	typedef T opd1_type;
	typedef T opd2_type;
	typedef bool ret_type;

	bool operator()(const T& v1, const T& v2)
	{return v1 == v2;}
};


template <> class less<char * > 
{
public:
	bool operator()(const char* v1, const char* v2)
		{return (bool)(strcmp(v1,v2) < 0);}
};

#ifdef _MSC_VER
#define strcasecmp stricmp
#endif

template <> class same<char * > 
{
public:
	bool operator()(const char* v1, const char* v2)
		{return (bool)(strcmp(v1,v2) == 0);}
};

// funzionale confronto non-case sensitive
class NoCase {
public:	
	bool operator()(const char* v1, const char* v2)
		{return (bool)(strcasecmp(v1,v2)<0);}
};

// funzionale confronto fra chiavi con predicato di confronto
template <class K, class T, class cmp> 
struct key_compare {
	typedef pair<K,T> paio;
	bool operator() (const paio & v1, const paio & v2)
		{ cmp c; return c(v1.first, v2.first);}
};

// operatore per il test di uguaglianza fra iteratori
template <class Forward, class Forward2, class Pred>
bool equal(Forward itb, Forward ite, Forward2 it2b, Forward it2e, Pred pr)
{
	while(itb != ite && it2b != it2e) {
		if(pr(*itb,*it2b) || pr(*it2b, *itb))
			return false;
		itb++;
		it2b++;
	}
	return itb == ite && it2b == it2e;
}

// confronto lessicale tra stringhe
// aaaBBC
// aaaBBCD
// * la prima e' minore della seconda: 
///	 1) quando elementi corrispondenti della prima sono minori di quelli della seconda
//   2) o quando e' piu' corta della seconda
template <class Forward, class Forward2,class Pred>
bool lexicomp(Forward itb, Forward ite, Forward2 it2b, Forward it2e, Pred pr)
{
	while(it2b != it2e && itb != ite) {
		if(pr(*itb, *it2b)) return true;
		if(pr(*it2b++, *itb++)) return false;		
	}
	return itb == ite && it2b != it2e;	
}
#endif