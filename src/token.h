// Nome: token.h
// Progetto: CPP/Lexer
// Autore: Ruffaldi Emanuele
// Descrizione: dichiarazione dei token e del contenitore di token
#ifndef TOKEN_H
#define TOKEN_H

#include <iostream.h>

enum TokenType { 
		tknNONE = 0, 
		tknIDENT, 
		tknICONST, 
		tknCCONST,
		tknFCONST,
		tknSTRCONST,
		tknFINEFILE,
		// bi-tri operators
		tknLTEQUAL,
		tknGTEQUAL,
		tknEQUAL,
		tknNEQUAL,
		tknSHIFTLEFT,
		tknSHIFTRIGHT,
		tknINCREMENT,
		tknDECREMENT,
		tknARROW,
		tknELLIPSE,
		tknLOGAND ,
		tknLOGOR,
		tknULTIMO1,
		// gli altri operatori e separatori corrispondono
		// al carattere che li rappresenta
		tknPLUS = '+',
		tknMINUS = '-',
		tknASSIGN = '=',
		tknLESS = '<', tknGREAT = '>',
		tknNOT = '!',
		tknSEMI = ';',
		tknCOLON = ':',
		tknCOMMA = ',',
		tknLPAREN = '(',
		tknRPAREN = ')',
		tknRBRACKET = '[',
		tknLBRACKET = ']',
		tknLBRACE = '{',
		tknRBRACE = '}',
		tknBINNOT = '~',

		// mentre le parole chiave sono i seguenti codici
		tknSECONDI = 200,
		tknBREAK = 200,
		tknIF,
		tknELSE,
		tknWHILE,
		tknCONST,
		tknDO,
		tknFOR,
		tknCONTINUE,
		tknINT,
		tknFLOAT,
		tknVOID,
		tknCHAR,
		tknSIZEOF,
		tknSTRUCT,
		tknRETURN,
		tknEXTERN,
		tknUltimo
		};


// Classe: Token
// Descrizione: questa classe e' un union type checked, poiche' raccoglie
//  e rende sicuro l'accesso ai dati propri di ogni tipo di token.
//  I dati che un token puo' memorizzare sono:
//   stringa
//	 identificatore
//	 costante char
//   costante int
//   costante float
//  Un oggetto di tipo token puo' essere inoltre convertito nel
//  TokenType memorizzato.
// 
// NOTA: si assume che la stringa passata a setString sia stata allocata
// dal chiamante con new; questo NON vale per setSymbol!
class Token {
public:
	Token();
	Token(const Token&);
	~Token();

	// informazioni sul tipo di token
    operator TokenType() const { return type;}	
	bool isEof() const {return type == tknFINEFILE;}	

	// informazioni sul dato del token 
	char *			getSymbol() const		{ return symbol;}
	const char *	getString() const		{return symbol;}
	int				getStringLength() const {return stringa.len;}
    int				getIConst() const		{return iconst;}
	float			getFConst() const		{return fconst;}
	char			getCConst() const		{return cconst;}

	// imposta i dati del token
	void setSymbol(const char * s);
	void setIConst(int iconst);
	void setTokenType(int t);
	void setString(char * s, int n);
	void setCConst(char c);
	void setFConst(float f);	
	
	// stampa testuale del token
	friend ostream& operator<< (ostream& o, const Token&);

private:

    TokenType  type;
    union {
        char*  symbol;		// un identificatore
		int    iconst;		// una costante int
		char   cconst;		// una costante char
		float  fconst;		// costante float
		struct {			// le stringhe C++ possono contenere anche
			char * str;		// il carattere \0, percio' occorre salvare
			int    len;		// anche la lunghezza
		} stringa;
    };

	// elimina la memoria allocata per stringhe e identificatori
	inline void cleanSymbol();
};

#endif