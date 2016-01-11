#include <iostream>
 #include "node.h"
#include "bools.h"
// Funzioni di output dei nodi

int CBlock::globalid = 0;
// indicizzata per nodo
char * nodeTable[] = {
	"Ndecl",		// dichiarazione, binary: sx = declspec, dx = lista dirdecl
	"Nargdecl",	// dichiarazione di argomento di funzione.
	"Ndirdecl",	// declarator, binary: sx = declarators, dx = init list
	"Nptrdecl",		// puntatore, CUnExpr
	"Narraydecl",		// array, CBinExpe: sx = size expr, dx = argument
	"Nfuncdecl",	// function, CBinList: sx = declarators del ritorno, dx =lista arg
	"Nstatic",
	"Nextern",
	"Nellipse",
	"Nadd", "Nsub", "Nmul", "Ndiv", "Nmod", "Nlogand", "Nbinor", "Nbinxor",
	"Nbinand", "Neq", "Nneq", "Nlt", "Ngt", "Nlteq", "Ngteq", "Nshl", "Nshr", 
	"Nassign", "Ncomma", "Nlogor","Ncall", "Ncast", 
	"Nlognot", "Nbinnot", "Nneg","Nconst","Nident","Narray",
	"Nptr", "Naddr",

	"Nexpr","Ndeclstat", "Nfuncbody", "Nblock", "Nreturn", "Nwhile", "Ndo",
	"Nif", "Nfor", "Nsymbol",
	"Norelse1", "Norelse2",  "Nexpr2bool", "Nbool2expr","Ncmp",
	"Ninc", "Ndec", "Nincpost", "Ndecpost",
};

// tabella usata per gli errori
char * nodeTable2[] = {
	"",		// dichiarazione, binary: sx = declspec, dx = lista dirdecl
	"",	// dichiarazione di argomento di funzione.
	"",	// declarator, binary: sx = declarators, dx = init list
	"",		// puntatore, CUnExpr
	"",		// array, CBinExpe: sx = size expr, dx = argument
	"",	// function, CBinList: sx = declarators del ritorno, dx =lista arg
	"static",
	"extern",
	"...",
	"+", "-", "*", "/", "%", "&&", "|", "^",
	"&", "==", "!=", "<", ">", "<=", ">=", "<<", ">>", 
	"=", "Ncomma", "Nlogor","Ncall", "Ncast", 
	"!", "~", "-","","","[]",
	"*", "&",

	"","", "", "", "return", "while", "do",
	"if", "for", "",
	"", "",  "", "","",
	"++", "--", "++", "--",
};


// Gestione della visualizzazione dell'albero:
// 
//  C:
//  |- Dev
//	||- Jdk
//  |||- Bin
//  ||\- Lib  
//  
//
// Level: 0x80000000 indica che e' l'ultimo dei figli del padre 
// i bit settati indicano che deve aggiungere una stanghetta
//
// CHILD(level)
// LASTCHILD(level): in tal caso 
// 

char * CNode::stringOf(NodeType op)
{
	return nodeTable2[op];
}





CConstExpr::CConstExpr(int i) :CExpression(Nconst)
{
	//cvalue(true);
	ival = i;
	ptipo = CType::Int();
}

CConstExpr::CConstExpr(char c):CExpression(Nconst)
{
//	cvalue(true);
	cval = c;
	ptipo = CType::Char();
}

CConstExpr::CConstExpr(float f):CExpression(Nconst)
{
//	cvalue(true);
	fval = f;
	ptipo = CType::Float();
}

CConstExpr::CConstExpr(const char * s, int len):CExpression(Nconst)
{
//	cvalue(true);
	strval.cp = s;
	strval.len = len;
	ptipo = (CType::Char())->ptr();
}



CBlock::CBlock(lineRef no, CBlock*p): CStatement(Nblock,no)
	{
		m_frameSize = 0;
		m_parent = p;
		m_nestingLevel = p ? m_parent->level()+1 : 0;
		m_blockID = globalid++;
	}


CStatement::CStatement(NodeType nt, lineRef no) : CNode(nt), lineno(no){}

CExprStat::CExprStat(NodeType nt, PExpression e,  lineRef num) : CStatement(nt, num)
	{
		expr = e;
		assert(nt == Nreturn || nt == Ndeclstat || nt == Nexpr);
		if(nt == Ndeclstat) assert(e->node() == Ndecl);
	}

// Function name	: CCondStat::mknode
// Description	    : creates a node for if, while, for, do..while
//	 statements.
// Return type		: CCondStat* 
// Argument         : NodeType nt
// Argument         : int lineno
// Argument         : PExpression e
// Argument         : PStatement s1 = 0
// Argument         : PStatement s2 = 0
CCondStat* CCondStat::mknode(NodeType nt, int lineno, PExpression e, PStatement s1 , PStatement s2 )
{
	assert(nt == Nif || nt == Nwhile);
	CCondStat* p = new CCondStat(nt, lineno);
	p->cond    = e;
	p->block   = s1;
	p->blockII = s2;
	return p;
}

CCondStat* CCondStat::mknode_for(int lineno, 
	PStatement before,
	PExpression cond,
	PExpression incr,
	PStatement body)
{
	CCondStat* p = new CCondStat(Nfor, lineno);
	p->cond    = cond;
	p->block   = before;
	p->blockII = body;
	p->incr	   = incr;
	return p;
}


CSymExpr::CSymExpr(CSymbol * sym) : CExpression(Nsymbol), m_symbol(sym) 
		{ptipo = sym->getTipo();}

CFuncBody::CFuncBody(int no, PBlock parent, PExpression e ) : 
	CStatement(Nfuncbody,no)
	{
		assert(e && *e == Ndecl);
		decl = e;
	}
	

#include "bools.h"

// **********************************************************************

void PrintVisitor::VisitCNode(CNode * pn)  
{
	spacer(o,level);
	o << nodeTable[*pn] << std::endl;
	doExprChild(pn);
}


void PrintVisitor::VisitCUnNode(CUnNode * pn)  
{ VisitCNode(pn);}

void PrintVisitor::doExprChild(CNode* pn)
{
	writeData tmp = level;
	if(pn->left()) {
		level = pn->right() ? CHILD(tmp) : LASTCHILD(tmp);
		pn->left()->accept(*this);
	}
	if(pn->right()) {
		level = LASTCHILD(tmp);
		pn->right()->accept(*this);
	}
	level  = tmp;

}

void PrintVisitor::VisitCExpression(CExpression * pn)  
{ 
	spacer(o,level);
	o << nodeTable[*pn] << " ";
	if(pn->ptipo) {
		o << " " << *(pn->ptipo) << " ";
	}
	if(pn->lvalue()) { o << "L"; }
	if(pn->cvalue()) { o << "C"; }
	o << std::endl;
	doExprChild(pn);
}

void PrintVisitor::VisitCUnExpr(CUnExpr * pn)  
{ VisitCExpression(pn);}

void PrintVisitor::VisitCBinExpr(CBinExpr * pn)  
{ VisitCExpression(pn);}


void PrintVisitor::VisitCComma(CComma * pn)  
{
	VisitCExpression(pn);
	CComma::iterator it = pn->begin();
	writeData tmp = level;
	while(it != pn->end()) {
		assert(*it);
		CComma::iterator tmpit = it;
		it++;				
		level = it == pn->end() ? LASTCHILD(tmp) : CHILD(tmp) ;
		(*tmpit)->accept(*this);		
	}
	level = tmp;
}

void PrintVisitor::VisitCDirDecl(CDirDecl * pn)  
{
	assert(pn->node() == Ndirdecl);
	spacer(o,level);
	o << nodeTable[*pn] << " " << (pn->identifer()? pn->identifer() : "") << '\n';
	doExprChild(pn);
}

void PrintVisitor::VisitCConstExpr(CConstExpr * pn)  
{ 
	spacer(o, level);
	o << "Nconst ";
	switch(pn->ptipo->getType()) {
	case tINT: o << "int = " << pn->ival; break;
	case tCHAR: o << "char = " << pn->cval; break;
	case tFLOAT : o << "float = " << pn->fval; break;
	case tPTR: o << "string"; break;
	default:
		std::cerr << "ERRORE INTERNO!";
		break;
	}
	o << std::endl;
}

void PrintVisitor::VisitCIdentifer(CIdentifer * pn)  
{
	spacer(o, level);
	o << "\"" << (pn->identifer()? pn->identifer() : "") << "\"" << std::endl;

}

void PrintVisitor::VisitCStatement(CStatement * pn)  
{ VisitCNode(pn);}

void PrintVisitor::VisitCBlock(CBlock * pn)  
{
	spacer(o,level);
	o << nodeTable[pn->node()] << std::endl;

	CBlock::iterator it = pn->begin();
	while(it != pn->end()) {
		assert(*it);
		CBlock::iterator tmpit = it;
		it++;
		writeData tmp = level;
		level =  it == pn->end() ? LASTCHILD(level) : CHILD(level);		
		(*tmpit)->accept(*this);
		level = tmp;
	}

}

void PrintVisitor::VisitCExprStat(CExprStat * pn)  
{ VisitCStatement(pn); }

void PrintVisitor::VisitCFuncBody(CFuncBody * pn)  
{	VisitCStatement(pn);}

void PrintVisitor::stampaLogic(LogicDesc & ld)
{
	static char * condt[] = {"cNIL", "cTRUE", "cFALSE", "cLT", 
	"cGT", "cLE", "cGE", "cEQ", "cNE"};

	
	o << "<" << condt[CondType(ld.condizione)] << ", ";
	
	// Qui c'e' il set
	
	if(ld.vero.empty())
		o << '*';
	else
		o << ld.vero;
	o << ',';
	if(ld.falso.empty())
		o << '*';
	else
		o << ld.falso;
	o << ',';
	if(ld.unisci.empty())
		o << '*';
	else
		o << ld.unisci;
	o << '>' << std::endl;
}

void PrintVisitor::VisitCBoolNode(CBoolNode * pn)  
{
	spacer(o, level);
	o << nodeTable[*pn] ;
	stampaLogic(pn->ldesc);
	doExprChild(pn);
}

void PrintVisitor::VisitCCondStat(CCondStat * pn)  
{
	spacer(o,level);
	o << nodeTable[pn->node()] << std::endl;
	writeData tmp = level;	
	switch(pn->node()) {
	case Nif:
		assert(pn->bcond);
		assert(pn->block);
		level = CHILD(tmp);
		pn->bcond->accept(*this);
		level = pn->blockII ? CHILD(tmp):LASTCHILD(tmp);
		pn->block->accept(*this);
		level =  LASTCHILD(tmp);
		if(pn->blockII) pn->blockII->accept(*this);
		break;
	case Nwhile:
		assert(pn->bcond && pn->block);
		level = CHILD(tmp);
		pn->bcond->accept(*this);
		level = LASTCHILD(tmp);
		pn->block->accept(*this);
		break;
	default:
		return;
	}
	level = tmp;
}

void PrintVisitor::VisitCSymExpr(CSymExpr * pn)  
{
	assert(pn->getSymbol()!= 0);
	spacer(o, level);
	o << "\"" << pn->getSymbol()->getNome() << "\" " << *(pn->getSymbol()->getTipo())<< "L \n";
}

void PrintVisitor::VisitCBoolExpr(CBoolExpr * pn)  
{ VisitCBoolNode(pn);}

void PrintVisitor::VisitCExpr2Bool(CExpr2Bool * pn)  
{ 
	VisitCBoolNode(pn);
}

void PrintVisitor::VisitCBool2Expr(CBool2Expr * pn)  
{ 
	VisitCExpression(pn);
}

void PrintVisitor::VisitCRelBoolExpr(CRelBoolExpr * pn)  
{ 
	VisitCBoolNode(pn);
}

