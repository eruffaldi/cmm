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


// #include <typeinfo.h>

// this should be a template member function

template <
	class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10>
void Format_output_1(std::ostream& o,
	Format<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>& fmt)
{
	assert(("No arg for output field", fmt.nCur< fmt.nParm));
    switch (++fmt.nCur) {
    case 1: o<< fmt.t1; break;
    case 2: o<< fmt.t2; break;
    case 3: o<< fmt.t3; break;
    case 4: o<< fmt.t4; break;
    case 5: o<< fmt.t5; break;
    case 6: o<< fmt.t6; break;
    case 7: o<< fmt.t7; break;
    case 8: o<< fmt.t8; break;
    case 9: o<< fmt.t9; break;
    case 10: o<< fmt.t10; break;
    }
}


template <class TExpected, class TActual>
TExpected cvt(TExpected*, TActual )
{
	std::cerr<<"Illegal conversion requested\n";
//    	<< typeid(TActual).name()<<" to "<<typeid(TExpected).name()<<endl;

	assert(("Illegal conversion requested", 0));
    return TExpected();
}
#define WRAP(expected, actual) inline expected cvt(expected*, actual x) {return (expected)x;}

WRAP(int, int)
WRAP(int, short)
WRAP(int, unsigned short)

WRAP(unsigned, unsigned)
WRAP(unsigned, short)
WRAP(unsigned, unsigned short)

WRAP(short, short)
WRAP(unsigned short, unsigned short)

WRAP(long, long)
WRAP(long, int)
WRAP(long, unsigned)
WRAP(long, short)
WRAP(long, unsigned short)

WRAP(unsigned long, unsigned long)
WRAP(unsigned long, int)
WRAP(unsigned long, unsigned)
WRAP(unsigned long, short)
WRAP(unsigned long, unsigned short)

WRAP(void*, void*)
WRAP(void*, const void*)
WRAP(const char*, char*)
WRAP(const char*, const char*)

// WRAP(int* const, int* const)

WRAP(float, float)
WRAP(float, int)
WRAP(float, unsigned)
WRAP(float, long)
WRAP(float, unsigned long)

WRAP(double, double)
WRAP(double, float)
WRAP(double, int)
WRAP(double, unsigned)
WRAP(double, long)
WRAP(double, unsigned long)

WRAP(long double, long double)
WRAP(long double, double)
WRAP(long double, float)
WRAP(long double, int)
WRAP(long double, unsigned)
WRAP(long double, long)
WRAP(long double, unsigned long)

WRAP(char, char)
WRAP(unsigned char, unsigned char)
WRAP(signed char, signed char)

#undef WRAP

template <class TExpected,
	class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10>
TExpected Format_get_1( TExpected*  x,
	Format<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>& fmt)
{
	assert(("No arg for output field", fmt.nCur< fmt.nParm));
    switch (++fmt.nCur) {
    case 1: return cvt(x, fmt.t1);
    case 2: return cvt(x, fmt.t2);
    case 3: return cvt(x, fmt.t3);
    case 4: return cvt(x, fmt.t4);
    case 5: return cvt(x, fmt.t5);
    case 6: return cvt(x, fmt.t6);
    case 7: return cvt(x, fmt.t7);
    case 8: return cvt(x, fmt.t8);
    case 9: return cvt(x, fmt.t9);
    case 10: return cvt(x, fmt.t10);

    }
    return 0;
}



template <class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10>
void Format<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>::
	do_output()
{
	enum eformat_t {eAsIs, eFlags, eWidth, ePrecision, eModifier, eType }
		eFormat= eAsIs; // state variable
	char modif= 0;
	int prec= -1;
    bool bSpaceFlag= false;

	char ch;

	istr.setf(0, std::ios::skipws);

	while ( istr.get(ch))
	{
		switch (eFormat) {
		case eAsIs:
			if (ch == '%')
				eFormat = eFlags;
			else
				os<< ch;
			break;
		case eFlags:
			switch (ch) {
			case '+':
				os.setf(std::ios::showpos);
				continue;
			case '-':
				os.setf(std::ios::left);
				continue;
			case '#':
				os.setf(std::ios::showbase | std::ios::showpoint);
				continue;
			case ' ':
				bSpaceFlag= true;
				continue;
			default:
				eFormat= eWidth;
			}

		case eWidth:
			{
				int w;
				eFormat= ePrecision;
				if (ch == '*')
				{
					w = Format_get_1((int*)0, *this);
					if (w<0) {
						os.setf(std::ios::left);
						w= -w;
					}
					os.width(w);
					continue;
				}	else {
					istr.putback(ch);
					istr>>w;
					if (istr) {
						if (ch == '0')
                        	os.fill('0');
						os.width(w);
						continue;
					} else {
						istr.clear();
						istr.get(ch);
					}
				}
			}
		case ePrecision:
			if (ch == '.')
			{
				istr.get(ch);
				if (ch == '*')
				{
					prec = Format_get_1((int*)0, *this);
                    if (prec>=0)
                    	os.precision(prec);
					continue;
				}	else {
					istr.putback(ch);
					istr>>prec;
                    if (prec>=0)
                    	os.precision(prec);
					if (istr) {
						continue;
					} else {
						istr.clear();
						istr.get(ch);
					}
				}
			}
			eFormat= eModifier;
		case eModifier:
			switch (ch) {
			case 'h':
			case 'l':
			case 'L':
				modif= ch;
				eFormat= eType;
				continue;
			default:
				eFormat= eType;
			}
		case eType:
			switch (ch) {
            case '*':
            	++nCur;
                os.setf(std::ios::dec);
                os.width(0);
                os.precision(6);
            case '@':
					if (prec == -1)
		               	Format_output_1(os, *this);
					else {
                    	std::ostrstream ostr;
   		               	Format_output_1(ostr, *this);
                        char* sz= ostr.str();
						if (int(strlen(sz)) >prec)
							sz[prec]=0;
						os<< sz;
						delete sz;
					}

                break;
			case 'd':
			case 'i':
				if (prec > -1) {
					os.width(prec);
					os.fill('0');
				}
				switch (modif) {
				case 'h':
                {
                	short x= Format_get_1((short*)0, *this);
                    if (bSpaceFlag && x>=0) os<<' ';
					os<<x;
                }
					break;
				case 'l':
                {
                	long x= Format_get_1((long*)0, *this);
                    if (bSpaceFlag && x>=0) os<<' ';
					os<<x;
                }
					break;
				default:
	                {
        	        	int x= Format_get_1((int*)0, *this);
            	        if (bSpaceFlag && x>=0) os<<' ';
    	                os<<x;
                	}
				}
				break;
			case 'o':
				os<<std::oct;
				break;
			case 'x':
			case 'X':
				os<<std::hex;
				if (ch=='X')
				{
					os.setf(std::ios::uppercase);
				}
				break;
			case 'u':
				if (prec > -1) {
					os.width(prec);
					os.fill('0');
				}
				switch (modif) {
				case 'h':
					os<<Format_get_1((unsigned short*)0, *this);
					break;
				case 'l':
					os<<Format_get_1((unsigned long*)0, *this);
					break;
				default:
					os<<Format_get_1((unsigned *)0, *this);
				}
				break;
			case 'f':
			case 'e':
			case 'E':
			case 'g':
			case 'G':
				switch (ch) {
				case 'f':
					os.setf(std::ios::fixed);
					break;
				case 'e':
					os.setf(std::ios::scientific);
					break;
				case 'E':
					os.setf(std::ios::scientific|std::ios::uppercase);
					break;
				case 'G':
					os.setf(std::ios::uppercase);
                    break;
				}
				if (prec > -1)
					os.precision(prec);
				switch (modif) {
				case 'L':
                {
                	long double x= Format_get_1((long double*)0, *this);
            	    if (bSpaceFlag && x>=0) os<<' ';
					os<<x;
                }
				break;
				default:
	            	{
        	        	double x= Format_get_1((double*)0, *this);
            	        if (bSpaceFlag && x>=0) os<<' ';
						os<<x;
    	            }
                }
                break;
			case 'c':
					os<<Format_get_1(( char*)0, *this);
					break;
			case 's':
					if (prec == -1)
						os<<Format_get_1((const char**)0, *this);
					else {
						char *sz= strdup(
                      	Format_get_1((const char**)0, *this));
						if (int(strlen(sz)) >prec)
							sz[prec]=0;
						os<< sz;
						delete sz;
					}
					break;
			case 'p':
					os<<Format_get_1((void**)0, *this);
					break;
			case '%':
					os<<ch;
					break;
			case 'n':
            	{
                int *pi= (int*)Format_get_1((int* const *)0, *this);
                *pi= os.tellp();
//            	assert(("Unsupported format %n", 0));
//				os.clear(ios::badbit|os.rdstate());
                // unsupported
                // tellp does not work on cout
				break;
                }
            }
			eFormat= eAsIs;
			os.flags(std::ios::skipws|std::ios::dec);
            os.precision(6);
            os.width(0); // unuseful
		}
	}
    assert(("Some arg not eaten", nParm == nCur));
    os<<std::ends;
}

//////////////////////////////////////////////////////////////

template <class Dummy>
void recur_comma(std::ostream& osz, int x, char comma, Dummy d)
{
	if (x<1000)
		osz<<x;
	else {
		recur_comma(osz, x/1000, comma, d);
		osz<<comma<<std::setw(3)<<std::setfill('0')<<(x%1000);
	}
}


template <class Dummy>
std::ostream& comma_print(std::ostream& ostr, double x,
	char comma, Dummy d)
{
	int w= ostr.width();
	char ch= ostr.fill();
	if (x>= 1000)
	{
		int y= int(x/1000);
		recur_comma(ostr,y ,comma, d);
		ostr<<comma;
		x-= y*1000;
	}
	ostr<<std::setw(3)<<std::setfill('0')<<x
		<<std::setw(w)<<std::setfill(ch);
	return ostr;
}