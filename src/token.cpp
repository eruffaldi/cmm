// Nome: token
// Progetto: CPP/Lexer
// Autore: Ruffaldi Emanuele
// Descrizione: implementazione dei contenitori di token
#include "token.h"

#include <string.h>

static char * nomi[] = {
	"tknNONE",	
	"tknIDENT", 
	"", "", "" , "", "", 
	"EOF",
	"<=", ">=", "==", "<<", ">>", "++", "--", 
	"->", "...", "&&", "||",
};
static char * altri[] = {
	"break", "if", "else", "while", "const", "do", "for", 
	"continue", "int", "float", "void", "char", "sizeof","return"				
};

ostream& operator<< (ostream& o, const Token& r)
{
	switch(r.type) {
		case tknCCONST: 
			if(r.getCConst() == '\n')
				o << "\'n\'";
			else
				o << "0x" << hex << int(r.getCConst()); 
			break;
		case tknIDENT:		o << r.getSymbol(); break;
        case tknICONST:		o << r.getIConst(); break;
		case tknSTRCONST:	o << '\"' << r.getString() << '\"'; break;
		case tknFCONST:		o << r.getFConst(); break;
		default: 
			if(r.type >= tknSECONDI && r.type < tknUltimo)
				o << altri[r.type-tknBREAK];
			else if(r.type <= tknULTIMO1)
				o << nomi[r.type];
			else
				o << char(r.type);
	}
    return o;
}

Token::Token() 
{
	type = tknNONE;
}

Token::~Token()
{
	cleanSymbol();
}

Token::Token(const Token&r)
{
	type = tknNONE;
	switch(r.type) {
	case tknIDENT: 
		setSymbol(r.getSymbol());
		break;
	case tknICONST:
		setIConst(r.getIConst());
		break;
	case tknSTRCONST:
		// qui richiede duplicazione
		{
			char  * cp = new char[r.getStringLength()];
			memcpy(cp, r.getString(), r.getStringLength());
			setString(cp, r.getStringLength());
		}
	case tknFCONST:
		setFConst(r.getFConst());
		break;
	case tknCCONST:
		setCConst(r.getCConst());
		break;
	default:
		type = r.type;
		break;
	}
}

inline void Token::cleanSymbol()
{
	if(type == tknIDENT)
		delete [] symbol;
	else if(type == tknSTRCONST) 
		delete[] stringa.str;
}

void Token::setSymbol(const char * s)
{
	cleanSymbol();
	symbol = new char[strlen(s)+1];
	strcpy(symbol, s);
	type = tknIDENT;
}

void Token::setString(char * s, int n)
{
	cleanSymbol();
	stringa.str = s;
	stringa.len = n;
	type = tknSTRCONST;
}

void Token::setIConst(int i)
{
	cleanSymbol();
	type = tknICONST;
	iconst = i;
}

void Token::setTokenType(int t)
{
	cleanSymbol();
	type = TokenType(t);
}

void Token::setCConst(char c)
{
	cleanSymbol();
	type = tknCCONST;
	cconst = c;
}

void Token::setFConst(float f)
{
	cleanSymbol();
	type = tknFCONST;
	fconst = f;
}
