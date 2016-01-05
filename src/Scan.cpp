// Nome: scan.cpp
// Progetto: CPP/Lexer
// Autore: Ruffaldi Emanuele
// Descrizione: implementazione del lexer
// History: 22/12/1998 child of Preproc

#include <iostream.h>
#include <string.h>
#include <iomanip.h>
#include <ctype.h>
#include <assert.h>
#include "Scan.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CScan::CScan(const char *filename, KeywordEntry keywords[], int nkeyw):
 CPreproc(filename)
{
	m_keywords = keywords;
	m_nkeyw = nkeyw;
	m_avaiable = 0;
	stateOk = 1; //ready();
}

CScan::~CScan()
{}

const Token& CScan::next()
{
	if(m_avaiable == 0)
		readToken();
	m_avaiable = 0;
	return m_ctoken;
}

 
void CScan::Error(ScanError sc, char * more)
{
	stateOk = false;
}

// ritorna true se il token e' del tipo richiesto
bool CScan::require(TokenType t)
{
	return current() == t;
}

//	Error(UNTERMCOMMENT, "unterminated multiline comment");

// getEscape
// Ritorna: il carattere della sequenza di escape
// Input: c		primo carattere della sequenza
//		  r		stream, usato per estrarre ulteriori caratteri
// Gestisce le sequenze normali, i numeri ottali e quelli esadecimali.
// Errori: 
//	UNEXPECTEDESCAPE	warning
//  TOOMANYCHAR			error
//  TOOBIGCHAR			error
//  ATLEASTANHEX		error
//  tutti questi errori sono recuperati in automatico dallo scanner.
//
char CScan::getEscape(char c)
{
	switch(c) {
	case '\'': return '\'';
	case '\"': return '\"';
	case '?': return '?';
	case '\\': return '\\';
	case 'a' : return '\a';
	case 'b': return '\b';
	case 'f': return '\f';
	case 'n' : return '\n';
	case 'r':  return '\r';
	case 't': return '\t';
	case 'v': return '\v';
	case 'x':
		{
			unsigned int v;
			char buf[20];
			int i = 0;
			if(!isxdigit(lowpeek())) {
				Error(ATLEASTANHEX, "hex constants must have at least one hex");
				return 0;
			}
			while(isxdigit(lowpeek())) {
				buf[i++] = lowget();
			}
			buf[i] = 0;
			istrstream r(buf, sizeof(buf));
			r >> hex >> v;			
			assert(r);
			if (v > 255) 
				Error(TOOBIGCHAR, "too big for character constant");						
			return char(v);
		}
		break;
	case '0':
	case '1': case '2':case '3':case '4':case '5':case '6':case '7':
		{
			char octbuf[4];			
			int j = 1;
			octbuf[0] = c;
			unsigned int v;

			while((c = lowpeek()) >= '0' && c <= '7' ) {
				if(j == 3) {
					Error(TOOMANYCHAR, "too many characters in constant");
					return 0;
				}
				lowget();
				octbuf[j++] = c;
			}		
			octbuf[j] = 0;						
			istrstream ist(octbuf, j+1);
			ist >> oct >> v;
			assert(ist);
			if (v > 255) 
				Error(TOOBIGCHAR, "too big for character constant");			
			return char(v);		
		}
	default: 
		errorString es;
		es << '\'' << c << "\' : unrecognized character escape sequence" << '\0';
		Error(UNEXPECTEDESCAPE, es);			
		return c;	
	}	
}

// scanString
// Input: sequenza di input posizionata sul primo carattere della stringa
// Output: definisce un token di tipo stringa
// Errori: 
//		UNEXPECTEDEOL
void CScan::scanString()
{
	ostrstream st;
	char c;
	do {
		while((c = lowpeek()) != '\"' && c != EOF) {
			lowget();
			if(c == '\\') 
				c = getEscape(lowget());
			else if(c == '\n' || c == '\r') {
				Error(UNEXPECTEDEOL, "unexpected end of line in string");	// close the string
				break;
			}
			st << c;
		}
		if(c == '\"') 
			lowget();
		
		// mangia gli spazi, se segue un'altro " concatena
		skipWhite();

		// controlla per un'altra stringa
		if(lowpeek() == '\"') 
			lowget();			
		else
			break;
	} while(1);
	st << '\0';
	m_ctoken.setString(st.str(), st.pcount());
}

bool CScan::scanChar()
{
	char c = lowget();
		if(c == '\n' || c == '\r') {
			Error(UNTERMLITCONST, "newline in constant");
			Error(EMPTYCONSTANT,  "char litteral vuota");			
			return false;
		}
		else {
			if(c == '\\') c = getEscape(lowget());
			m_ctoken.setCConst(c);
			while((c = lowpeek()) != '\'') {
				if(c == '\n' || c == '\r') {	
					Error(UNTERMLITCONST, "newline in constant");
					c = get();
					break;
				}
				c = lowget();
			}
			if(c == '\'') lowget();		
		}
		return true;
}
void CScan::readToken() 
{
char c;
bool anotherToken = false;		// da settare se scarto il token corrente

	if(!stateOk) return;
	m_avaiable = 1;
	if(eof()) {
		m_ctoken.setTokenType(tknFINEFILE);
		return;
	}
  
	do {
		while((c = get()) == ' ')
			;
		if(c == EOF) {
			m_ctoken.setTokenType(tknFINEFILE);
			return;
		}
		m_startline = lineno();
		// Keyword or Identifer
		if(isalpha(c) || c == '_') {
			char id[256];
			TokenType tkn;
			int i = 0;
			id[i++] = c;
			while((c = lowpeek()) != 0 && (isalnum(c) || c == '_')) {
				id[i++] = lowget();
			}
			id[i++] = 0;
			if((tkn = whichKeyword(id)) == tknNONE) 			
				m_ctoken.setSymbol(id);		
			else 
				m_ctoken.setTokenType(tkn);
		}
		else if(c == '.' || isdigit(c)) 
			scanNumber(c);	
		else if(c == '\'') 
			anotherToken = !scanChar();
		else if(c == '\"') 
			scanString();	
		else {	
			// dobbiamo controllare per gli operatori lunghi
			// perquesto osserviamo il carattere successivo
			// oltre ad un flag che consente di controllare gli
			// operatori di assegnamento
			char c2 = lowpeek();
			TokenType tkn = TokenType(c);
			bool canassign = false;
			switch(c) {
				case '+':
				case '-':
					if(c2 == c) {
						lowget();
						tkn = c == '+' ? tknINCREMENT : tknDECREMENT;
					}
					else if(c2 == '>' && c == '-') {
						lowget();
						tkn = tknARROW;
					}
					else {
						tkn = c== '+' ? tknPLUS: tknMINUS;
						canassign = true;
					}
					break;
				case '=':
					if(c2 == '=') {
						lowget();
						tkn = tknEQUAL;
					}
					else
						tkn = tknASSIGN;
					break;
				case '<':
				case '>':
					if(c2 == '=') {
						lowget();
						tkn = c == '<' ? tknLTEQUAL : tknGTEQUAL;
					}
					else if(c2 == c) {
						lowget();
						tkn = c == '<' ? tknSHIFTLEFT : tknSHIFTRIGHT;
						canassign = true;
					}
					else {
						tkn = c == '<' ? tknLESS : tknGREAT;
						canassign = true;
					}
					break;
				case '!':	// logical not or not equal
					if(c2 == '=') {
						lowget();
						tkn = tknNEQUAL;
					}
					else
						tkn = tknNOT;
					break;
				 case ';':
				 case ':':
				 case ',':
				 case '(':
				 case ')':
				 case '[':
				 case ']':
				 case '{':
				 case '}':
				 case '~':
					 tkn = TokenType(c);
					break;
				 case '&':	
				 case '|':
					if(c2 == c) {
						lowget();
						tkn = c == '&' ? tknLOGAND : tknLOGOR;
					}
					else
						canassign = true;
					break;
				 case '*':
				 case '/':
				 case '%':
				 case '^':
					canassign = true;
					break;
				 default:

					// errore, carattere non riconosciuto.
					errorString es;
					es << "unknown character \'0x" << hex << int(c) << "\'" << ends;
					Error(UNKCHAR, es);
					anotherToken = true;
					break;
			}
			// se canassign e peek e' = allora lo stesso token deve essere
			// op assign
			m_ctoken.setTokenType(tkn);
		}	
	  }
	  while(anotherToken);
}

char CScan::digitSequence(ostream &o)
{
	char c;
	while((c = lowpeek()) != EOF && isdigit(c))  {
		o << c;
		lowget();
	}	
	return c;
}

void CScan::scanNumber(char c)
{
// numero, lo mettiamo in uno stream per vedere 
// se e' floating o integer
// floating-constant:
//	 fractional-const exponent-part OPT 
//	 digitsequence exponent-part
char buf[40];
ostrstream st(buf, sizeof(buf));
bool isfloat = false;	

	st << c;
	// . digit-seq  or  simple dot or ... (ellipse)
	if(c == '.') {
		if(isdigit(lowpeek())) {
			isfloat = true;
			c = digitSequence(st);						
		}
		else {
			if(lowpeek() == '.') {
				lowget();
				if(lowpeek() == '.') {
					lowget();
					m_ctoken.setTokenType(tknELLIPSE);
				}
				else
					lowback('.');
			}
			else
				m_ctoken.setTokenType('.');		
			return;
		}
	}
	else {			
		c = digitSequence(st);
				
		// dot, allora seguita da seconda digit sequence OPT
		if(c == '.') {
			isfloat = true;
			st << char(lowget());
			c = digitSequence( st);			
		}
		
	}
	
	// exponent part
	if(c == 'e' || c == 'E') {
		isfloat = true;
		st << c;
		lowget();		// scarta E o e
		c = lowpeek();
		if(c == '+' || c == '-') {
			st << c;
			lowget();	// scarta +-
		}
		digitSequence( st);	// ultima digit seq
	}

	st << '\0';
	istrstream is(buf, sizeof(buf));
	if(isfloat) {
		float f;
		is >> f;
		m_ctoken.setFConst(f);
	}
	else {
		int i;
		is >> i;
		m_ctoken.setIConst(i);
	}
	if(!is) 
		Error(BADCONST, "bad constant value");
}

TokenType CScan::whichKeyword(const char *k)
{
	// usare binSearch
	for(int i = 0; i < m_nkeyw; i++) {
		if(strcmp(k, m_keywords[i].text) == 0)
			return m_keywords[i].tkn;
	}
	return tknNONE;
}

