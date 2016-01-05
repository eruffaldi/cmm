// Nome: parsedecl.cpp
// Progetto: CPP/Parser
// Autore: Ruffaldi Emanuele
// Descrizione: parser di dichiarazioni
#include <string.h>
#include <iostream.h>
#include "parser.h"
#include "cmm.h"

// DECL
// scanner visibile come sc. o come estensione di CScan!
// 1* come estensione di cscan


// Function name	: parseFuncDecl
// Description	    : analisi sintattica di una dichiarazione di funzione
// Return type		: PFuncDecl 
// Argument         : PExpression dirdeclritorno
//
// Regola sintattica
//	( [argdecl-clist] [...] )
PComma CParser::parseFuncDecl(PExpression dirdeclritorno)
{	
	PComma funzione(new CComma(Nfuncdecl, dirdeclritorno));
	
	assert(current() == '(');
	next();

	while(current() != ')') {
		if(current() == tknELLIPSE) {
			next();
			require(')','(');
			CExpression * pe = new CExpression(Nellipse);
			pe->ptipo = CType::Ellipse();
			funzione->appendChild(pe);
			return funzione;
		}
		else {
			// deve fare il parsing della variabile sen
			funzione->appendChild(parseArgDecl());
			if(current() == ',') 
				next();
			else
				break;
		}
	}

	if(current() != ')') {		
		if(current() == '{') {
			CMM::error(" missing \')\' before \'{\'");
		}
		else
			CMM::error(" syntax error in formal parameter list");			
	}	
	next();
	return funzione;
}



// Function name	: CParser::parseDeclSpec
// Description	    : 
// Return type		: PExpression
// Argument         : PType & tp
//
// declspec:
//		int|char|void|float		uno solo fra questi
//		static | extern			uno solo fra questi
PExpression CParser::parseDeclSpec(PType & tp)
{
	tp = 0;
	do {
		PType ti = 0;
		switch(current()) {
		case tknINT: ti = CType::Int(); break;
		case tknCHAR: ti = CType::Char(); break;
		case tknFLOAT: ti = CType::Float(); break;
		case tknVOID: ti = CType::Void(); break;
		default:
			return PExpression();
		}
		next();
		if(ti && tp) {
			CMM::error("Doubled specified DECLSPEC");
			break;
		}
		else if(ti)
			tp = ti;
	} while(1);
	return PExpression();
}

// Function name	: CParser::parseArgDecl
// Description	    : analisi sintattica di argomento di funzione
// Return type		: PBinExpr 
//
// Differisce da un normale Declaration per il nodo ritornato (Nargdecl) e
// per il comportamento con la ',' provoca il ritorno
//
// Sintassi
//	declspec dirdecl
PBinExpr CParser::parseArgDecl()
{
	PDirDecl pdd = new CDirDecl;
	PBinExpr p = new CBinExpr(Nargdecl,PExpression(), pdd);
	
	p->setLeft(parseDeclSpec(p->ptipo));
	pdd->setLeft(parseDeclarators(pdd));
	if(current() == '=') {
		CMM::error("Initialization of variables or default values not supported\n");
	}
	return p;
}

// Function name	: parseDeclarators
// Description	    : effettual l'analisi sintattica di "declarators"
// Return type		: PExpression 
// Argument         : char* & ident
//
// Regola sintattica
// declarator:
//		* declarator		==> Nptr(d) 
//		declarator [ const_expr ]	==> Narray(d,ce) 
//		declarator ( comma_expr )	==> Nfuncdecl(d, parametri)
//		( declarator )				==> d
//		identifer					==> ident = valore identificatore
//
// Errori
//  solo riflessi da altre funzioni
PExpression CParser::parseDeclarators(PDirDecl & dd)
{
	assert(dd);
	PExpression prima, top;
	while(current() == '*') {
		prima = new CUnExpr(Nptrdecl, prima);
		next();
	}

	if(current() == '(') {
		next();
		top = parseDeclarators(dd);
		require(')', '(');
	}
	else
		top = PExpression();

	if (dd->identifer() == 0 && current() == tknIDENT) {		
		dd->identifer() = installaIdent(current().getSymbol());
		dd->setLineNumber(lineno());
		next();
	}

	do {
		if(current() == '[') {
			PBinExpr first, last;

			while(current() == '[') {
				next();
				PBinExpr p = new CBinExpr(Narraydecl);
				p->setRight(parseExpression());
				require(']','[');
				
				if(first == 0) 
					last = first = p;
				else {
					last->setLeft(p);
					last = p;									
				}
			}			
			last->setLeft(prima);
			prima = first;
		}
		else if(current() == '(') {
			prima = parseFuncDecl(prima);
		}
		else
			break;
	} while(1);

	if(top) {
		// scorri la dichiarazione top fino a trovare l'ultimo elemento
		// per agganciare prima		
		// es. * (*k)(func);
		PExpression tmp = top;
		while(tmp->left() != 0)
			tmp = (PExpression)tmp->left();
		tmp->setLeft(prima);
		return top;
	}
	else
			return prima;

}


// Function name	: CParser::parseDeclaration
// Description	    : 
// Return type		: PExpression 
//
// Regola sintattica
//	declaration:
//		declspec	dirdecl [ , dirdecl]
//	dirdecl:
//		declarators [= initexpression ]
//
PExpression CParser::parseDeclaration()
{
	
	PComma p = new CComma(Ndecl);
	
	p->setLeft(parseDeclSpec(p->ptipo));
	do {
		PDirDecl pdd = new CDirDecl;
	
		pdd->setLeft(parseDeclarators(pdd));
		p->appendChild(pdd);
		if(current() == '=') {
			CMM::error("Initialization of variables or default values not supported");
			next();
		}
		else if(current() == ',') 
			next();					
		else 
			break;
	} while(1);
	return p;
}




bool CParser::canBeDeclaration(TokenType tkn)
{
	static int cbexpr[] = {
	tknINT, tknCHAR, tknVOID, tknFLOAT, tknEXTERN, tknSTRUCT};
	for(int i = 0;i < sizeof(cbexpr)/sizeof(cbexpr[0]); i++)
		if(tkn == cbexpr[i]) return true;
	return false;

}


