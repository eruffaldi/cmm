// Nome: nparser.cpp
// Progetto: CPP/Parser
// Autore: Ruffaldi Emanuele
// Descrizione: modulo principale di parser
//
// Gestione degli errori:
//	dentro il parser gli errori vanno solamente stampati 
//	non lanciamo una eccezione
#include <iostream.h>
#include <string.h>
#include "parser.h"
#include "keywords.h"
#include "cmm.h"
#include "format.h"


// il costruttore stampa l'errore 
class par_exception: public cmm_exception
{
public:
	par_exception(ErrorCode ec) {
		ostrstream os;
		os << CMM::lookupError(ec)<< ends;
		_str = os.str();		
	}

	par_exception(ErrorCode ec, const char *s ) {
		ostrstream os;
		os << format(CMM::lookupError(ec),s) << ends;
		_str = os.str();		
	}

	par_exception(ErrorCode ec, int c ) {
		ostrstream os;
		os << format(CMM::lookupError(ec),char(c)) << ends;
		_str = os.str();		
	}

	par_exception(char * text = 0) : _str(text) { }
	~par_exception() {delete[] _str;}
	virtual const char * what() const throw() {return _str;}
protected:
	void setError(char * cp) {_str = cp;}
private:
	char * _str;
};

void CScanOnly::Error(ScanError r, char * more)
{
	CScan::Error(r, more);
	ostrstream os;
	os << "lexer " << r << ": " << more << ends;
	CMM::error(os.str());
	delete os.str();
}

CScanOnly::CScanOnly(const char * filename)
: CScan(filename,KeywordList, sizeof(KeywordList)/sizeof(KeywordList[0]))
{
}

CParser::CParser(const char * filename) : CScanOnly(filename)
{
}



// Function name	: CParser::installaIdent
// Description	    : 
// Return type		: char * 
// Argument         : const char * cp
char * CParser::installaIdent(const char * cp)
{
	char * p = new char[strlen(cp)+1];
	strcpy(p, cp);
	return p;
}


// Function name	: CParser::require
// Description	    : 
// Return type		: void 
// Argument         : TokenType tknreq
// Argument         : TokenType fromwho
void CParser::require(int tknreq, int fromwho)
{
	if(current() == tknreq) {
		next();
		return;
	}
	throw par_exception(errREQUIREDTOKEN,tknreq);
}

bool CParser::canBeStatement(TokenType tkn)
{
static int cbexpr[] = {
	tknWHILE, tknFOR, tknDO, tknRETURN, tknIF};
	for(int i = 0;i < sizeof(cbexpr)/sizeof(cbexpr[0]); i++)
		if(tkn == cbexpr[i]) return true;
	return false;
}

// Function: CParser::parse
// Description: topmost function of the parser. Parses
//   the stream of token as a translation unit
PBlock CParser::parse() 
{
	PBlock p = new CBlock(0,0);	
	currentBlock = p;
	next();

	while(current() != tknFINEFILE) {
		if(!canBeDeclaration(current())) {
			throw par_exception(errREQUIREDDECL);
			next();
			continue;
		}
		int n = lineno();
		PExpression pe = parseDeclaration();
		if(!pe) continue;	
		assert(*pe == Ndecl);

		// deve essere terminata da una ;
		if(current() == ';') {			
			p->appendChild(new CExprStat(Ndeclstat, pe, n));
			next();			
		}
		// definizione di funzione
		else if(current() == '{') {
			// stabilire se e' una function declaration
			if(isFuncBodyDecl(pe)) {
				PFuncBody pf = new CFuncBody(n, p,pe);
				pf->block = (PBlock)parseStatement();
				p->appendChild(pf);
			}
			else {
				delete pe;
				throw par_exception(errBODYWITHOUTFUNC);
			}
		}
		else 
			throw par_exception(errEXPSEMICOMMA);
			
	}
	return p;
}


bool CParser::isFuncBodyDecl(PExpression pe)
{
	assert(pe && *pe == Ndecl);
	PComma pc = (PComma)pe;
	int n = 0;
	CComma::iterator it = pc->begin();

	while(it != pc->end()) {
		n++;
		if(n > 1) return false;
		PDirDecl pdd = *it;
		assert(*pdd == Ndirdecl);

		if(!pdd->identifer()) return false; // MISSING NAME
		if(!pdd->left()) return false;		// NOT A FUNCTION
		if(pdd->left()->node() != Nfuncdecl)	// NOT A FUCN
			return false;
		it++;
	}
	return true;
}

// 

// Function name	: CParser::parseStatement
// Description	    : parses a C++ statement, this can be
//   an expression-statement or another statement. 
// Return type		: PStatement 
//
// Syntax:
//
// statement: 
//	expression ;
//	{ statement statement ... }
//  if ( expression ) statement [ else statement ]
//  while ( expression ) statement
//  do statement while ( expression ) ; 
//  ;
// Only the statement and the declarations store the line
// number, so we must care of them
PStatement CParser::parseStatement()
{
PExpression expr;

	while(1) {
		int lno = lineno();
		if(canBeExpression(current())) {
			expr = parseExpression();
			require(';',0);
			if(expr) 
				return new CExprStat(Nexpr, expr, lineno());		
		}		
		else if(current() == '{') {
			// costruisce un nuovo blocco, lo inserisce nella lista
			// del padre e diventa il blocco corrente.
			// L'attributo globale currentBlock deve essere
			// ripristinato in uscita
			PBlock pb = new CBlock( lno, currentBlock);
			currentBlock->appendBlock(pb);
			currentBlock = pb;

			next();
			while(current() != '}' && current() != tknFINEFILE) {
				pb->appendChild(parseStatement());
			}
			require('}', '{');

			currentBlock = pb->parent();
			return pb; 
		}
		else if(canBeStatement(current())) {
			PStatement pstat,pstat2;
			PExpression pincr;			

			switch(current()) {
			case tknIF:						
				next();
				require('(', tknIF);
				expr = parseExpression();
				require(')', tknIF);
				pstat = parseStatement();
				if(current() == tknELSE) {
					next();
					pstat2 = parseStatement();
				}				
					
				return CCondStat::mknode(Nif, lno, expr, pstat, pstat2);

			case tknWHILE:
				next();
				require('(', tknWHILE);
				expr = parseExpression();
				require(')', tknWHILE);
				return CCondStat::mknode(Nwhile, lno, expr, 
					parseStatement());				

			case tknFOR:
				next();						require('(', tknFOR);				
				pstat = parseStatement();	require(';', tknFOR);				
				expr  = parseExpression();	require(';', tknFOR);
				pincr = parseExpression();  require(')', tknFOR);
				pstat2= parseStatement();	
				return CCondStat::mknode_for(lno, pstat,
					expr, pincr, pstat2);

			case tknDO:
				next();
				pstat = parseStatement();	
				require(tknWHILE, tknDO);
				require('(', tknDO);
				expr  = parseExpression();	
				require(')', tknDO);
				require(';', tknDO);
				return CCondStat::mknode(Ndo, lno, expr, pstat);
				
			case tknRETURN:
				next();
				expr = parseTopExpression();
				require(';',tknRETURN);
				return new CExprStat(Nreturn , expr, lno);

			default:
				{
					ostrstream os;
					os << "statement token " << current() << " not yet implemented"<<ends;
					CMM::error(os.str());
					delete os.str();
				}
				next();
			}
		}
		else if(canBeDeclaration(current())) {
			int lno = lineno();
			expr = parseDeclaration();
			if(!expr) continue;	
			assert(*expr == Ndecl);
			require(';', 0);
			return new CExprStat(Ndeclstat, expr, lno);
		}
		else {
			{
				ostrstream os;
				os << "unknown token " << current() << " in body"<<ends;
				CMM::error(os.str());
				delete os.str();
			}
			next();		
		}
	}

}

const Token& CParser::next()
{
	const Token & tkn = CScanOnly::next();
	CMM::setLineRef(fileno(), lineno());
	return tkn;
}