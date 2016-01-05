/*
 *
 * Copyright (c) 1997
 * Infomedia Ed.
 *
 * www.infomedia.it
 *
 * Permission to use, copy, modify, distribute and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and
 * that both that copyright notice and this permission notice appear
 * in supporting documentation.  Infomedia Ed. makes no
 * representations about the suitability of this software for any
 * purpose.  It is provided "as is" without express or implied warranty.
 *
 *
*/

/*
	Here are some handful routines fo format output:
	- format(char*, ...): emulates printf
	- comma(T n, char comma = ','): prints large number in comma format,
    	i.e. 10000-> "10,000"
	- repeat(const char* s, int n): manipulator to repeat any string n times
    	i.e. cout<<repeat(10, "*") -> "**********"


    The most interesting is, of course, format(). I tried to have it
    work in the same way as printf does.
    The major limitation is that you cannot have >10 args. You could
    raise the limit, but some limit must exist. 10 is just fine for my purposes.
    To my knowledge,
    the only unsupported printf's standard format is %n, because I did
    not found a way to know the number of chars output to an ostream insofar.
    Types are checked at run time, i.e. format("%d", "Hello") causes
    a run time error (assert() failed) - but I did not used RTTI.
    It would possible to generate a compile time error, but this would
    require writing much more boilerplate code.
    Other run time errors are: trying to use an inexisting arg and
	leaving an unused arg. My format() is quite conservative. Look for
    assert() if you want to change this policy.


    I did one extension: '@' is used as a type field to print
    an object;
    For example: cout<<format("%10@", myObj)<<endl;
    This prints myObj in a 10 char field; myObj must support the insertion
    on an ostream (i.e. operator<<(ostream&, const T&) ).
    Of course, you can use %@ to print any native type: i.e.
    format("%4.4@", 3.14) works - it prints 3.14 using << on exactly 4 chars
    However, be aware that the meaning of the precision field dcoes not depend on the type
    of the arg to be printed. If you use the precision field then @, the
    programm will use the precision field as it is defined for strings i.e.
    as the max number of chars to print. My rationale was that this formatting
    is quite useful but is missing from ios. Precision in the floating point
    sense is more intuitive but it is less useful - I cannot see any point in
    writing format("%4.4@", 3.14) if this means the same as format("%4.4g", 3.14).
    format("%4.4@", 3.14) is a shortcut for:
    ostrstream ostr;
    ostr<<3.14;
	format("%4.4s", ostr.str())
    <use format>
    delete ostr.str()

    The code has been tested on Borland BC++ 5.0.1, but it does not contain
    any Borland- or Windows- specific code.
    The code is completely templatized. So you better try it on
    a compiler with a decent template support.

    Any comment, suggestion and bug report should be addressed to:
    Graziano Lo Russo, lorusso@programmers.it
    I do not guarantee that I will fix it - but I will try!

   	Warning: I do not claim that this software is in any way useful.
    DISCLAIMER:
    Trying to figure out how this software works can seriously damage your brain.

	Adapted by: Ruffaldi Emanuele 
*/

#ifndef __FORMAT_H
#define __FORMAT_H
#include <iostream.h>
#include <iomanip.h>
#include <strstrea.h>
#include <assert.h>


//////////////////////////////////////////////////////////

template <class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10>
struct Format {
	istrstream istr;
    int nParm;
    const T1& t1;
    const T2& t2;
	const T3& t3;
    const T4& t4;
    const T5& t5;
    const T6& t6;
	const T7& t7;
    const T8& t8;
    const T9& t9;
    const T10& t10;
    int nCur;
    ostrstream os;

	Format(const char* _szFormat, int _nParm,
    const T1& _t1, const T2& _t2,
	const T3& _t3, const T4& _t4, const T5& _t5, const T6&  _t6,
	const T7& _t7, const T8& _t8, const T9& _t9, const T10&  _t10):
	istr(const_cast<char*>(_szFormat)), nParm(_nParm),
    t1(_t1), t2(_t2),  t3(_t3), t4(_t4), t5(_t5),
    t6(_t6), t7(_t7),  t8(_t8), t9(_t9), t10(_t10), nCur(0)
    {do_output(); }

    ~Format() {delete os.str();}
    void printOn(ostream& ostr) const
    { ostr<< ((ostrstream&)os).str(); }

/*
// BC++ 5.0 does not like friend templates, so I had to make Format a struct

  	friend template <class T1, class T2, class T3, class T4, class T5,
		class T6, class T7, class T8, class T9, class T10>
	void Format_output_1(ostream& o,
		Format<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>& fmt);

	friend template <class TExpected,
		class T1, class T2, class T3, class T4, class T5,
		class T6, class T7, class T8, class T9, class T10>
	TExpected Format_get_1(ostream& o,
		const Format<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>& fmt,
	    class TExpected *)
*/

    void do_output() ;

};


template <class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10>
inline ostream& operator<<(ostream& o,
	const Format<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>& fmt)
{
	((Format<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>&)fmt).printOn(o);
    return o;
}

#include "format_impl.h"


////////////////// Helper functions ///////////////////////////

template <class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10>
inline Format<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>
format(const char* szFormat, const T1& t1, const T2& t2,
	const T3& t3, const T4& t4, const T5& t5, const T6&  t6,
	const T7& t7, const T8& t8, const T9& t9, const T10&  t10)
{ return Format<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>
		(szFormat, 10, t1, t2, t3, t4, t5, t6, t7, t8, t9, t10);
}

template <class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9>
inline Format<T1, T2, T3, T4, T5, T6, T7, T8, T9, int>
format(const char* szFormat, const T1& t1, const T2& t2,
	const T3& t3, const T4& t4, const T5& t5, const T6&  t6,
	const T7& t7, const T8& t8, const T9& t9)
{ return Format<T1, T2, T3, T4, T5, T6, T7, T8, T9, int)
		(szFormat, 9, t1, t2, t3, t4, t5, t6, t7, t8, t9, 0);
}

template <class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8>
inline Format<T1, T2, T3, T4, T5, T6, T7, T8, int, int>
format(const char* szFormat, const T1& t1, const T2& t2,
	const T3& t3, const T4& t4, const T5& t5, const T6&  t6,
	const T7& t7, const T8& t8)
{ return Format<T1, T2, T3, T4, T5, T6, T7, T8, int, int)
		(szFormat, 9, t1, t2, t3, t4, t5, t6, t7, t8, 0, 0);
}

template <class T1, class T2, class T3, class T4, class T5,
	class T6, class T7>
inline Format<T1, T2, T3, T4, T5, T6, T7, int, int, int>
format(const char* szFormat, const T1& t1, const T2& t2,
	const T3& t3, const T4& t4, const T5& t5, const T6&  t6,
	const T7& t7)
{ return Format<T1, T2, T3, T4, T5, T6, T7, int, int, int)
		(szFormat, 8, t1, t2, t3, t4, t5, t6, t7, 0, 0, 0);
}

template <class T1, class T2, class T3, class T4, class T5,
	class T6>
inline Format<T1, T2, T3, T4, T5, T6, int, int, int, int>
format(const char* szFormat, const T1& t1, const T2& t2,
	const T3& t3, const T4& t4, const T5& t5, const T6&  t6)
{ return Format<T1, T2, T3, T4, T5, T6, int, int, int, int)
		(szFormat, 6, t1, t2, t3, t4, t5, t6, 0, 0, 0, 0);
}


template <class T1, class T2, class T3, class T4, class T5>
inline Format<T1, T2, T3, T4, T5, int, int, int, int, int>
format(const char* szFormat, const T1& t1, const T2& t2,
	const T3& t3, const T4& t4, const T5& t5)
{ return Format<T1, T2, T3, T4, T5, int, int, int, int, int)
		(szFormat, 5, t1, t2, t3, t4, t5, 0, 0, 0, 0, 0);
}

template <class T1, class T2, class T3, class T4>
inline Format<T1, T2, T3, T4, int, int, int, int, int, int>
format(const char* szFormat, const T1& t1, const T2& t2,
	const T3& t3, const T4& t4)
{ return Format<T1, T2, T3, T4, int, int, int, int, int, int>
		(szFormat, 4, t1, t2, t3, t4, 0, 0, 0, 0, 0, 0);
}

template <class T1, class T2, class T3>
inline Format<T1, T2, T3, int, int, int, int, int, int, int>
format(const char* szFormat, const T1& t1, const T2& t2,
	const T3& t3)
{ return Format<T1, T2, T3, int, int, int, int, int, int, int>
		(szFormat, 3, t1, t2, t3, 0, 0, 0, 0, 0, 0, 0);
}

template <class T1, class T2>
inline Format<T1, T2, int, int, int, int, int, int, int, int>
format(const char* szFormat, const T1& t1, const T2& t2)
{ return Format<T1, T2, int, int, int, int, int, int, int, int>
		(szFormat, 2, t1, t2, 0, 0, 0, 0, 0, 0, 0, 0);
}

template <class T1>
inline Format<T1, int, int, int, int, int, int, int, int, int>
format(const char* szFormat, const T1& t1)
{ return Format<T1, int, int, int, int, int, int, int, int, int>
		(szFormat, 1, t1, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}

inline Format<int, int, int, int, int, int, int, int, int, int>
format(const char* szFormat)
{ return Format<int, int, int, int, int, int, int, int, int, int>
		(szFormat, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
}
/*
	Examples:
	cout<<format("%s\n", "Hello World");
	cout<<format("%10.4f", 3.14);
*/


//////////////////////////////////////////////////////////////////////

template <class Dummy>
struct RepeatPair {
	const char* s;
	unsigned n;
	RepeatPair(	const char* as, unsigned an):
		s(as), n(an) {}
	void print(ostream& o);
};

template <class Dummy>
void RepeatPair<Dummy>::print(ostream& o)
{
	for (int n1 = n; n1>0; n1--)
		o << s;
}

inline RepeatPair<int> repeat(const char* s, int n)
	{ return RepeatPair<int> (s, n); }

inline ostream& operator<<(ostream& ostr, RepeatPair<int>& r)
	{ r.print(ostr); return ostr;}

// Example: 	cout<<repeat("*", 10)<<endl;

//////////////////////////////////////////////////////////////////////

struct comma_parm {
	double m_n;
	char m_comma;
	comma_parm(double n, char comma):
		m_n(n), m_comma(comma) {}
};


inline comma_parm comma(double n, char comma)
	{ return comma_parm (n, comma); }

inline ostream& operator<<(ostream& ostr, comma_parm& x)
	{ return comma_print(ostr, x.m_n, x.m_comma, 0);}
	

// Example: cout<<comma(10000)<<endl;


#endif