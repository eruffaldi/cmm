// Nome: scan
// Progetto: CPP/Lexer
// Autore: Ruffaldi Emanuele
// Descrizione: analizzatore lessicale
#ifndef SCAN_H
#define SCAN_H

#ifdef _MSC_VER
#include <strstrea.h>
#else
#include <strstream.h>
#endif

#include "preproc.h"
#include "errors.h"
#include "token.h"

class CScan  : public CPreproc
{
public:
	struct KeywordEntry {
		char *		text;
		TokenType	tkn;
	};

	// Costruttore dello Scanner, occorre il file di input
	// e le parole chiave.
	enum ScanError {
		UNTERMCOMMENT,	// commento non terminato, errore
			// raggiunto eof in un commento, il parsing continua
		UNEXPECTEDEOF,	// fine del file non attesa
			// il parsing puo' continuare
		BADCONST,		// costante errata		
			// anche qui il parsing continua, il valore della costante sara' casuale
		UNKCHAR,			// char inatteso, error
			// carattere ignorato
		UNEXPECTEDEOL,		// stringa non terminata, error
			// la stringa viene chiusa
		UNTERMLITCONST,		// char litteral non terminato, error
			// il carattere viene chiuso
		EMPTYCONSTANT,		// char litteral vuoto, error
			// la char litteral e' 0
		UNEXPECTEDESCAPE,	// 'character' di una sequenza inattesa, warning
			// ritorno character
		TOOMANYCHAR,		// troppi char in una costante, error
			// ignoro i maggiori
		TOOBIGCHAR,			// valore del char troppo grande, error
			// valore casuale
		ATLEASTANHEX,		// almeno una hex digit, error
			// ritorna 0
	};

	CScan(const char *filename, KeywordEntry keywords[], int n);
	virtual ~CScan();
	virtual void Error(ScanError, char * more = 0);

	
	
	// estrazione di token
    virtual const Token&	next();
    const Token&	current() const		{return m_ctoken;}
    bool			require(TokenType);    
	
	
	operator bool() {return stateOk;}
	void Error(ScanError e,errorString & ost) { Error(e, (char*)ost);}

protected:
	KeywordEntry *	m_keywords;
	int				m_nkeyw;
    int				m_startline;
    Token			m_ctoken;		
    bool			m_avaiable;		
	bool			stateOk;

	void		readToken();
	bool		scanChar();
	void		scanNumber(char c);
	void		scanString();
	char		digitSequence(ostream &o);
	char		getEscape(char c);
	TokenType	whichKeyword(const char *);

};

#endif 
