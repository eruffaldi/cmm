// Nome: parse_expr.cpp
// Progetto: CPP/Parser
// Autore: Ruffaldi Emanuele
// Descrizione: parser di espressioni
#include "parser.h"
#include "cmm.h"

enum BinExprState {
	stTOP,
	stASSIGN,
	stLOGOR,
	stLOGAND,
	stBINOR,
	stBINXOR,

	stBINAND,
	stRELEQ,
	stREL,
	stSHIFT,
	stADD,
	stMUL,
	stUNK,		// error
	stPREC,		// state has less precedence
};

struct ExprTransition {
	int	token;
	BinExprState next;
	NodeType nt;
};

ExprTransition ttran[] = {
	'=', stASSIGN, Nassign,
	tknLOGOR, stLOGOR, Nlogor,
	tknLOGAND, stLOGAND, Nlogand,
	'|', stBINOR, Nbinor,
	'^', stBINXOR, Nbinxor,
	'&', stBINAND,Nbinand,
	tknEQUAL, stRELEQ,Neq,
	tknNEQUAL, stRELEQ,Nneq,
	'<', stREL,Nlt,
	'>', stREL, Ngt,
	tknLTEQUAL, stREL, Nlteq,
	tknGTEQUAL, stREL, Ngteq,
	tknSHIFTLEFT, stSHIFT, Nshl,
	tknSHIFTRIGHT, stSHIFT, Nshr,
	'+', stADD, Nadd,
	'-', stADD, Nsub,
	'*', stMUL, Nmul,
	'/', stMUL, Ndiv,
	'%', stMUL, Nmod,
};

BinExprState tran(BinExprState src, TokenType tkn, NodeType& nt)
{
	for(int i = 0; i < sizeof(ttran)/sizeof(ttran[0]); i++)
		if(ttran[i].token == tkn) {
			nt = ttran[i].nt;
			if(src == stASSIGN && ttran[i].next == src) {
				return stASSIGN;	// right associative
			}
			else
				return ttran[i].next> src ? ttran[i].next: stPREC;
		}
	return stUNK;
}


// Function name	: CParser::parseTopExpression
// Description	    : 
// Return type		: PExpression 
PExpression CParser::parseTopExpression()
{
	return parseCommaExpression();
}


// Function name	: CParser::parseCommaExpression
// Description	    : 
// Return type		: PExpression 
PExpression CParser::parseCommaExpression()
{
	PExpression p;
	PComma cm;
	p = parseExpression();
	if(current() == ',') {
		cm = new CComma(Ncomma);		
		cm->appendChild(p);
		do {
			next();
			p = parseExpression();
			cm->appendChild(p);
		} while(current() == ',');
 		return cm;
	}
	else 
		return p;
}

// Function name	: CParser::parseExpression
// Description	    : 
// Return type		: PExpression 
PExpression CParser::parseExpression()
{
		return parseBinary(stTOP);
}


// Function name	: CParser::parseBinary
// Description	    : 
// Return type		: PExpression 
// Argument         : BinExprState st
PExpression CParser::parseBinary(int st)
{
	PExpression p = parsePrimary();
	do {
		NodeType nt;
		BinExprState ns = tran((BinExprState)st, current(), nt);
		if(ns == stPREC || ns == stUNK) return p;
		next();
		PExpression q = parseBinary(ns);

		assert(q != 0);
		p = new CBinExpr(nt, p, q);
	} while(1);

}

// Parsing di una primary expression.

// Function name	: CParser::parsePrimary
// Description	    : 
// Return type		: PExpression 
PExpression CParser::parsePrimary()
{
	PExpression p;

	// unary-exprsssion:
	//		postfix-expression
	//		++ unary-expression
	//		-- unary-expression
	// postfix-expression:
	//		primary-expression
	//		postfix [ expr ]
	//		postifix (expr-list)
	//      postfix ++
	//		postfix ++
	// primary-expression --> parseElement
	if(current() == tknINCREMENT) {
		next();
		p = new CUnExpr(Ninc, parsePrimary());
	}
	else if(current() == tknDECREMENT) {
		next();
		p = new CUnExpr(Ndec, parsePrimary());
	}
	else
		p = parseElement();

	if(!p) return PExpression();

	PExpression q;
	assert(p != 0);

	while(1) {
		switch(current()) {
		case '[':
			next();
			q = parseTopExpression();
			require(']', '[');
			p = new CBinExpr(Narray, p, q);
			break;
		case '(':
			// function call
			next();
			q = parseCommaExpression();
			require(')', '(');
			p = new CBinExpr(Ncall, p, q);
			break;
		case tknINCREMENT:
			next();
			p = new CUnExpr(Nincpost, p);
			break;
		case tknDECREMENT:
			next();
			p = new CUnExpr(Ndecpost, p);
			break;
		default:
			return p;
		}
	}
}


// Function name	: CParser::parseElement
// Description	    : 
// Return type		: PExpression 
PExpression CParser::parseElement()
{
	PExpression p;	
	switch(current()) {
	case '(': // cast or parentesis
		next();
		if(canBeDeclaration(current())) {
			// do a cast
			p = parseDeclaration();
			require(')', '(');
			p = new CBinExpr(Ncast, p, parsePrimary());
		}
		else {
			p = parseTopExpression();
			require(')', '(');
		}
		return p;
	case '!':
		next();
		return new CUnExpr(Nlognot, parsePrimary());
	case 26: // tilde
		next();
		return new CUnExpr(Nbinnot, parsePrimary());
	case '*':
		next();
		return new CUnExpr(Nptr, parsePrimary());
	case '&':
		next();
		return new CUnExpr(Naddr, parsePrimary());
	case '+':
		next();
		return parsePrimary();
	case '-':
		next();
		return new CUnExpr(Nneg, parsePrimary());		
	case tknCCONST:   p = mkleaf_c(current().getCConst()); break;
	case tknICONST:   p = mkleaf_c(current().getIConst()); break;
	case tknFCONST:	  p = mkleaf_c(current().getFConst()); break;
	case tknSTRCONST: p = mkleaf_c(current().getString(), current().getStringLength()); break;
	case tknIDENT:    p = mkleaf_ident(current().getSymbol()); break;
	default:
		{
			ostrstream os;
			os << "unknown token " << current() << " in expression"<<ends;
			CMM::error(os.str());
			delete os.str();
		}	
		return PExpression();
	}	
	next();
	return p;
}

// inizio di una espressione: vanno tutti bene eccetto l'identificatore,
//  poiche' se supportiamo classi/struct/union/typedef potrebbe essere 
//  l'identificatore di un tipo.

// Function name	: CParser::canBeExpression
// Description	    : 
// Return type		: bool 
// Argument         : TokenType tkn
bool CParser::canBeExpression(TokenType tkn)
{
static int cbexpr[] = {
	'+','-','!', 26, '&', '*', tknIDENT, '(', tknCCONST,
	tknFCONST, tknICONST, tknSTRCONST, tknINCREMENT, tknDECREMENT};
	for(int i = 0;i < sizeof(cbexpr)/sizeof(cbexpr[0]); i++)
		if(tkn == cbexpr[i]) return true;
	return false;
}

 PExpression CParser::mkleaf_c(char c)
{
	return new CConstExpr(c);
}

 PExpression CParser::mkleaf_c(float f)
{
	return new CConstExpr(f);
}

 PExpression CParser::mkleaf_c(int i)
{
	return new CConstExpr(i);
}

 PExpression CParser::mkleaf_c(const char *s, int n)
{
char * tmp = new char[n];
	memcpy(tmp, s, n);
	return new CConstExpr(tmp, n);
}

 PExpression CParser::mkleaf_ident(const char *s)
{
	return new CIdentifer(installaIdent(s));
}
