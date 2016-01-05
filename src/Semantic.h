// Nome: semantic
// Progetto: CPP/Semantic
// Autore: Ruffaldi Emanuele
// Descrizione: analizzatore semantico
#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "node.h"
#include "symbol.h"
#include "errors.h"
#include "bools.h"
#include "cmm.h"

class Semantic {
	PBlock currentBlock;
	CFunction * currentFunction;
	
	TempLabel tmplabelCount;	// conteggio label virtuali

	// current function information
	bool  bReturned;	// Sinthetized Attribute
	bool  bReturnSaw;
public:
	Semantic();
	void analyze(PBlock);	
	void setLineNo(int l);
	
private:
	// function member.
	typedef PNode (Semantic::* semHandler)(PNode  p);
	static semHandler tbl[Nlast];
	

	PNode process(PNode  p);
	void exprproc(PExpression & p);
	

	struct SymbolRedefinition {
		CSymbol * psymbol;		
	};

	// Element Handlers
	PNode doNIdent(PNode);

	// Declarations
	PNode doNptrdecl(PNode);
	PNode doNdecl(PNode);
	PNode doNarraydecl(PNode);
	PNode doNdirdecl(PNode);
	PNode doNfuncdecl(PNode);
	PNode doNargdecl(PNode);
	
	// Expression
	PNode doNpreincdec(PNode);
	PNode doNpostincdec(PNode);
	PNode doNassign(PNode);
	PNode doNptr(PNode);
	PNode doNaddr(PNode);
	PNode doNarray(PNode);
	PNode doNadd(PNode);
	PNode doNmul(PNode);
	PNode doNbinop(PNode);
	PNode doNbinnot(PNode);
	PNode doNneg(PNode);
	PNode doNcomma(PNode);
	PNode doNcast(PNode);
	PNode doNcall(PNode);

	// Translation unit level
	PNode doNfuncbody(PNode);
	PNode doNblock(PNode);
	
	// Boolean Processing
	CExpression * bool2expr(PNode p);
	PBoolNode boolproc(PExpression pe);
	PNode doNlogand(PNode);
	PNode doNlogor(PNode);
	PNode doNlnot(PNode);	
	LabelSet generaLabel();
	PNode doNcmp(PNode);

	PNode doRecurse(PNode);

	// Statements
	PNode doNexpr(PNode);
	PNode doNif(PNode);
	PNode doNwhile(PNode);
	PNode doNfor(PNode);
	PNode doNreturn(PNode);

	// Symbol Management
	CSymbol * findSymbol(char * ident);
	CSymbol * installaVariabile(char * ident, PType typo,int lineno,  bool isStatic = false);
	CFunction * installaFunzione(char * ident, PType typo,int lineno);
	
};

// Typecheking class
class TypeCheck {
	static bool x86target;
public:
	static void convert(PExpression & srcExpr, PType toType, bool forced = false);
	static PType bestType(PExpression p1, PExpression p2);
	static PFunction assignable(PType target, PType source);
	static int typeSize(PType t);
	static PFunction binary(NodeType nt, PType source, PType target);

	static void setAXP() {x86target = false;}
	static void setx86() {x86target = true;}
};

class CConstEvaluator {
public:
	static PConstExpr eval(NodeType op, PExpression & s1, PExpression & s2);
	static PConstExpr eval(NodeType op, PExpression & s1);
	static bool evalAsBool(PConstExpr);
	static PConstExpr cast(PExpression & s1, PType tipo);
};


#endif
