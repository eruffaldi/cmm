// Nome: parser.h
// Progetto: CPP/Parser
// Autore: Ruffaldi Emanuele
// Descrizione: dichiarazione del parser
#ifndef PARSER_H
#define PARSER_H

#include "node.h"
#include "Scan.h"


class CScanOnly: public CScan
{
public:
	CScanOnly(const char *cp);		
	virtual void Error(ScanError, char * more);
};


// The CParser class processes the token extraced by the CScan and
// generates a syntax tree. The syntax tree is made of objects of
// classes derived from the CNode class; at this level the CParser
// cannot handle data type and symbols.
// The parser is implemented as a descentent recursive parser,
// the non terminals are mapped into specific functions, this is
// true except for the binary operators that are handled through
// a table of precedence.
//
// Usage:
//   the parser can be constructed passing a filename and then
//   activated with the member function parse(). The parse() function
//   parses the source and returns the translation unit as 
//   an object CBlock. 
class CParser : public CScanOnly {



	// Statement Parser
	bool isFuncBodyDecl(PExpression);
	virtual const Token&	next();
	
	// Declaration Parser
	PComma parseFuncDecl(PExpression);
	PExpression parseDeclSpec(PType & tp);
	PBinExpr parseArgDecl();
	PExpression parseDeclarators(PDirDecl & dd);

	// Expression Parser
	PExpression parseElement();
	PExpression parsePrimary();
	PExpression parseBinary(int st);
	PExpression parseCommaExpression();
	PExpression parseTopExpression();
	PExpression parseInitExpression();

	char * installaIdent(const char * cp);
	void require(int, int);

	PBlock currentBlock;

	// Node Generators
	PExpression mkleaf_c(char);
	PExpression mkleaf_c(float);
	PExpression mkleaf_c(int);
	PExpression mkleaf_c(const char*, int);
	 PExpression mkleaf_ident(const char *s);
public:
	CParser(const char *filename);	
	PExpression parseDeclaration();
	PExpression parseExpression();
	PStatement parseStatement();	
	bool canBeExpression(TokenType tkn);
	bool canBeDeclaration(TokenType tkn);	
	bool canBeStatement(TokenType tkn);
	PBlock parse();		
};

#endif
