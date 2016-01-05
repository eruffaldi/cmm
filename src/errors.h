// Nome: errors.h
// Progetto: CPP/Base
// Autore: Ruffaldi Emanuele
// Descrizione: gestori eccezioni di base
#ifndef ERRORS_H
#define ERRORS_H

#ifdef _MSC_VER
#include <strstrea.h>
#else
#include <strstream>
#endif
#include "errorTable.h"

// questa classe si comporta come un ostrstream ma supporta l'autodistruzione
// della stringa memorizzata
class errorString: public std::ostrstream
{	
public:
	typedef char * tcp;
	operator tcp () {return str();}
	~errorString() {delete [] str();}
};

#ifndef _EXCEPTION_
#define _EXCEPTION_

// classe di base delle eccezioni
class mexception {
public:
    mexception() throw() {}
    mexception(const mexception& rhs) throw() {}
    mexception& operator=(const mexception& rhs) throw() {return *this;}
    virtual ~mexception() throw() {}
    virtual const char *what() const throw() = 0;
    };

#endif

class cmm_exception: public mexception {
public:	
};



#endif 