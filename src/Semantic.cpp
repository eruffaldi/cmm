// Nome: semantic
// Progetto: CPP/Semantic
// Autore: Ruffaldi Emanuele
// Descrizione: analizzatore semantico
#include <iostream.h>
#include "dvector.h"
#include "semantic.h"
#include "format.h"
// Exception Handlers

class sem_exception: public cmm_exception
{
public:
	sem_exception(ErrorCode ec) {
		ostrstream os;
		os << CMM::lookupError(ec)<< ends;
		_str = os.str();		
	}

	sem_exception(ErrorCode ec, const char *s ) {
		ostrstream os;
		os << format(CMM::lookupError(ec),s) << ends;
		_str = os.str();		
	}

	sem_exception(char * text = 0) : _str(text) { }
	~sem_exception() {delete[] _str;}
	virtual const char * what() const throw() {return _str;}
protected:
	void setError(char * cp) {_str = cp;}
private:
	char * _str;
};

class cannot_convert_error : public sem_exception
{
public:
	cannot_convert_error(ErrorCode ec, NodeType op, PType p1, PType p2)
	{
		ostrstream o, o1, o2;
		//o1 << *p1 << ends;
		// o2 << *p2 << ends;
		p1->fullNameType(o1);
		p2->fullNameType(o2);
		o1 << ends;
		o2 << ends;
		o << format(CMM::lookupError(ec), CNode::stringOf(op), o1.str(), o2.str()) << ends;;
		delete o1.str();
		delete o2.str();
		setError(o.str());	
	}

	// function call argument mismatch
	cannot_convert_error(ErrorCode ec, int index, PType p1, PType p2)
	{
		ostrstream o, o1, o2;
		o1 << *p1 << ends;
		o2 << *p2 << ends;
		o << format(CMM::lookupError(ec), index, o1.str(), o2.str()) << ends;;
		delete o1.str();
		delete o2.str();
		setError(o.str());
	}

};

class operator_error: public sem_exception
{
public:
	operator_error(NodeType op, ErrorCode ec)
	{
		ostrstream o;
		o << format(CMM::lookupError(ec), CNode::stringOf(op)) << ends;;
		setError(o.str());	
	}
};

//************************************************************************
CondType Cond::tbl[9] = {cNIL, cFALSE, cTRUE, cGE, cLE, cGT, cLT, cNE, cEQ};
	
void Semantic::setLineNo(int l) { CMM::setLineRef(0,l);}

// funzioni per l'analisi semantica dei vari nodi
// tabella ad acesso diretto per velocizzare
Semantic::semHandler Semantic::tbl[Nlast] = 
{
	&doNdecl, // Ndecl
	&doNargdecl, // Nargdecl
	&doNdirdecl, // Ndirdecl
	&doNptrdecl, // Nptrdecl
	&doNarraydecl, // Narraydecl
	&doNfuncdecl, // Nfuncdecl
	0, // Nstatic, ignored
	0, // Nextern, ignored
	0, // Nellipse, ignored
	
	&doNadd,&doNadd,&doNmul,&doNmul,&doNmul,	// Nadd, Nsub, Nmul, Ndiv, Nmod, 
	&doNlogand, // Nlogand, 
	&doNbinop,&doNbinop,&doNbinop, //Nbinor, Nbinxor, Nbinand
	&doNcmp,&doNcmp,&doNcmp,&doNcmp,&doNcmp,&doNcmp, // Neq, Nneq, Nlt, Ngt, Nlteq, Ngteq 
	&doNbinop,&doNbinop,// Nshl, Nshr
	&doNassign,&doNcomma,&doNlogor,&doNcall,&doNcast,&doNlnot,&doNbinnot,&doNneg,// Nassign, Ncomma, Nlogor, Ncall, Ncast, Nlognot, Nbinnot, Nneg,
	0,				// Nconst (doesn't require processing)
	&doNIdent,		// Nident
	&doNarray,		// Narray
	&doNptr,		// Nptr,
	&doNaddr,		// Naddr
	&doNexpr,		// Nexpr
	&doRecurse,		// Ndeclstat
	&doNfuncbody,   // Nfuncbody
	&doNblock,		// Nblock
	&doNreturn,		// Nreturn
	&doNwhile,				// Nwhile
	&doNwhile,   // Ndo,
	&doNif,   // Nif
	&doNfor,   //Nfor, 
	0,	// Nsymbol
	0,0,0,0,0,
	&doNpreincdec, &doNpreincdec, &doNpostincdec,
	&doNpostincdec,
};
Semantic::Semantic()
{ tmplabelCount =0 ;}


// Function name	: analyze
// Description	    : effettua l'analisi semantica del programma
// Return type		: void 
// Argument         : PBlock pb
void Semantic::analyze(PBlock pb)
{
	currentBlock = pb;
	currentFunction = 0;
	process(pb);
}



// Function name	: exprproc
// Description	    : 
// Return type		: PExpression 
// Argument         : PExpression p
//
// processa il nodo
// lo converte ad expression se boolean
// lo ritorna typechecked
//
void Semantic::exprproc(PExpression& p)
{
	if(!p) return ;
	assert(p->isExpr());
	p = process(p);
	if(p->isBool()) 
		p = bool2expr(p);	
}

PNode Semantic::process(PNode  p)
{
	if(!p) return p;
	if(tbl[(int)p->node()] == 0) return p;
	return (this->* (tbl[(int)p->node()]))(p);
}


// risale lungo tutti i blocchi
CSymbol * Semantic::findSymbol(char * ident)
{
	PBlock pb = currentBlock;
	while(pb != 0) {
		if(pb->getSymTab().containsKey(ident)) {
			return pb->getSymTab().get(ident);
		}
		else 
			pb = pb->parent();
	}
	return 0;
}


// Function name	: installFunzione
// Description	    : 
// Return type		: CFunction * 
// Argument         : char * ident
// Argument         : PType typo
// Argument         : int lineno
CFunction * Semantic::installaFunzione(char * ident, PType typo,int lineno)
{
	assert(!findSymbol(ident));
	assert(currentBlock);
	CFunction * nuovo = new CFunction(ident, typo, lineno, currentFunction);
	currentBlock->getSymTab().put(ident, nuovo);	
	nuovo->proto = true;
	nuovo->setSType(stoEXTERN);
	return nuovo;
}

// installa la variabile dentro al blocco corrente
// TODO: extern e static flags
CSymbol * Semantic::installaVariabile(char * ident, PType typo, int lineno, bool isStatic )
{
	assert(!findSymbol(ident));
	assert(currentBlock);
	CSymbol * nuovo = new CSymbol(ident, typo, lineno, currentFunction);	
	currentBlock->getSymTab().put(ident, nuovo);
	return nuovo;
}


// HANDLERS
PNode Semantic::doNIdent(PNode p)
{
	assert(p && p->node() == Nident);
	PIdentifer pi = p;
	CSymbol * sym = findSymbol(pi->identifer());
	if(!sym) {		
		// installa variabile per evitare altri errori
		// di seguito
		sym = installaVariabile(pi->identifer(), CType::Int(),0, false);
		throw sem_exception(errSYMBOLNOTFOUND,pi->identifer());
	}
	CSymExpr * pe = new CSymExpr(sym);
	sym->used++;

	pe->ptipo = sym->getTipo();
	pe->lvalue(!sym->getTipo()->isArray());
	delete p;
	return pe;
}

PNode Semantic::doNdirdecl(PNode p)
{
	assert(p && *p == Ndirdecl);
	PDirDecl pdd = p;
	if(pdd->left()) {	// trasferimento dell'attributo tipo
		if(pdd->lineNumber() != 0) setLineNo(pdd->lineNumber());		
		PExpression(pdd->left())->ptipo = pdd->ptipo;
		process(PExpression(pdd->left()));
		pdd->ptipo = PExpression(pdd->left())->ptipo;
	}
	return pdd;
}

// Function name	: doNdecl
// Description	    : 
// Return type		: PNode unchanged
// Argument         : PNode (PComma)
//

// richiama ricorsivamente le Ndirdecl per il calcolo del
// tipo corretto. Per ogni elemento costruisce un simbolo 
PNode Semantic::doNdecl(PNode p)
{
	assert(p && *p == Ndecl);
	PComma pc = (PComma)p;
	CComma::iterator it = pc->begin();
	while(it != pc->end()) {		
		assert(*it && **it == Ndirdecl);
		PDirDecl pdd = (PDirDecl)*it;
		PSymbol psym;

		pdd->ptipo = pc->ptipo;
		process(pdd);
		if(!pdd->identifer())
			;
		else if((psym = findSymbol(pdd->identifer())) != 0) {
			if(psym->isFunction()) {
				// confronta i tipi devono essere UGUALI
				if(psym->ptipo != pdd->ptipo) 
					throw sem_exception(errFUNCPROTOREDEF, pdd->identifer());				
			}
			else
				throw sem_exception(errSYMBOLREDEF, pdd->identifer());				
		}
		else {	
			if(pdd->left() && pdd->left()->node() == Nfuncdecl) 
				installaFunzione(pdd->identifer(),pdd->ptipo, pdd->lineNumber());
			else
				installaVariabile(pdd->identifer(),pdd->ptipo, pdd->lineNumber(),false);			
		}
		it++;
	}
	return p;
}




// Function name	: Semantic::doNptrdecl
// Description	    : 
// Return type		: PNode 
// Argument         : PNode p
//
// trasferisce il tipo al nodo figlio se presene
// al ritorno arrichiesce dello stato ptr
PNode Semantic::doNptrdecl(PNode p)
{
	assert(p && *p == Nptrdecl);
	PUnExpr pe = (PUnExpr)p;
	if(pe->left()) {
		PExpression(pe->left())->ptipo = pe->ptipo;
		process(pe->left());
		pe->ptipo = PExpression(pe->left())->ptipo;
	}
	pe->ptipo = pe->ptipo->ptr();
	return pe;
}
	

// Function name	: Semantic::doNarraydecl
// Description	    : calcolo tipo per array
// Return type		: PNode 
// Argument         : PNode p (PBinExpr)
//
//	 sx.tipo = tipo
//	 process sx
//	 tipo = array(dx.costante, sx.tipo)
//   
PNode Semantic::doNarraydecl(PNode p)
{
	assert(p && *p == Narraydecl);
	PBinExpr pe = p;
	long size;

	if(pe->sx()) {	
		pe->sx()->ptipo = pe->ptipo;
		process(pe->sx());		
		pe->ptipo = pe->sx()->ptipo;
		if(pe->ptipo->isFunc()) throw sem_exception(errFUNCCANNOTBEARRAY);					
	}
	if(pe->dx()) {
		pe->dx() = process(pe->dx());
		TypeCheck::convert(pe->dx(), CType::Int());
		try {
			size = PConstExpr(pe->dx())->ival;
		}
		catch(bad_dynamic_cast) {
			throw sem_exception(errEXPECTEDCONST);			
		}
	}
	pe->ptipo = pe->ptipo->array(size);
	return pe;
}

// Function name	: Semantic::doNfuncdecl
// Description	    : serve a costruire la struttura del tipo 
// Return type		: PNode 
// Argument         : PNode p (PComma)
// Semantica:
//		sx.tipo = tipo
//		process sx
//		tipo = sx.tipo
//		calocola prototipo
//		tipo = proto
PNode Semantic::doNfuncdecl(PNode p)
{
	assert(p && *p == Nfuncdecl);
	PComma pe = p;
	if(pe->sx()) {
		pe->sx()->ptipo = pe->ptipo;
		process(pe->sx());
		pe->ptipo = pe->sx()->ptipo;
	}	

	int idx = 0;
	int nArgomenti        = pe->conta();
	CComma::iterator it = pe->begin();
	PType * proto = new PType[nArgomenti];
	try {	
		bool bEllipsed = false;
		for(;it != pe->end();it++) {
			if(**it == Nellipse) {
				bEllipsed = true;
				proto[idx++] = PExpression(*it)->ptipo;
				it++;
				if(it != pe->end()) {
					throw sem_exception(errARGSAFTERELLIPSE);
					
				}
				break;

			}			
			else {
				assert(*it && **it == Nargdecl);
				process(*it);
				proto[idx++] = PExpression(*it)->ptipo;
			}		
		}
		pe->ptipo = pe->ptipo->func(proto,nArgomenti, csCdecl);	
	}
	catch(...) {		
		delete[] proto;
		throw;
	}
	return p;
}

// Funzione: doNargdecl
// Descrizione: Dichiarazione di un argomento di funzione:
// Semantica:
//   tipo = dx.tipo
PNode Semantic::doNargdecl(PNode p)
{
	assert(p && *p == Nargdecl);
	PBinExpr pe = p;
	if(pe->right()) {
		PExpression(pe->right())->ptipo = pe->ptipo;
		process(pe->right());
		pe->ptipo = PExpression(pe->right())->ptipo;
	}	
	return pe;
}



// Function name	: Semantic::doNfuncbody
// Description	    : 
// Return type		: PNode 
// Argument         : PNode p  (PFuncBody)
//
// La dichiarazione viene inserita come prototipo dentro la
// tabella dei simboli, poi viene riprocessata la funzione cosi'
// da allocare i nomi dei parametri come simboli del blocco della
// funzione
PNode Semantic::doNfuncbody(PNode p)
{
	assert(p && *p == Nfuncbody);
	PFuncBody pb = (PFuncBody)p;
	PFunction pfunc;
	PComma pfuncdecl;
	try {
		bReturned = bReturnSaw = false;		
		process(p->left());	
		// il simbolo viene installato ma se era gia' presente 
		// genero una eccezione
	}
	catch(SymbolRedefinition sr) {
		throw sem_exception(errFUNCREDEF, sr.psymbol->getNome());
		return p;
	}
	
	assert(pb->decl);
	{	
	PComma pc = (PComma)pb->decl;	
	CComma::iterator it = pc->begin();
	assert(it != pc->end());

	PDirDecl pdd = (PDirDecl)*it;
		assert(*pdd == Ndirdecl && pdd->identifer() && pdd->left() &&
			pdd->left()->node() == Nfuncdecl);
	pfuncdecl = (PComma)pdd->left();
	pfunc = (CFunction*)findSymbol(pdd->identifer());
	}
	assert(pfunc && pfunc->isFunction());
	pfunc->proto = false;
	if(pfunc->sType() == stoEXTERN)
		pfunc->setSType(stoNORMAL);
	pfunc->codice = pb->block;
	pfunc->inited = true;

	
	// occorre spiegare alla ricerca dei simboli di cercarli
	// nella tabella del blocco della funzione
	currentBlock = pb->block;

	CComma::iterator it = pfuncdecl->begin();
	pfunc->argSize = 0;
	int idx = 0;
	while(it != pfuncdecl->end()) {
		if(**it == Nellipse) 
			cout << "TODO: Ellipse not yet implemented in analysis\n";
		else {				
			PBinExpr pe = (PBinExpr)*it;
			PDirDecl pdd = (PDirDecl)pe->right();
			if(pdd->identifer()) {
				CSymbol * psym = findSymbol(pdd->identifer());
				if(psym) 
					throw sem_exception(errSYMBOLREDEF, pdd->identifer());
				else {
					psym = installaVariabile(pdd->identifer(),
						pdd->ptipo, pdd->lineNumber(),false);
					psym->setSType(stoPARAM);				
					psym->index = idx++;
					// cout << "Installed " << pdd->identifer() << endl;
					// ALL PARAMETERS ARE 4 bytes IN SIZE!
					pfunc->argSize += 4;
				}
			}
			
		}
		it++;
	}
	
	// ripristina il blocco corrente, verra' modificato da Nblock
	currentBlock = pb->block->parent();

	// setta la funzione corrente, per la diagnostica
	currentFunction = pfunc;

	// processa il blocco
	assert(p->right());
	process(p->right());

	// Return Type Check
	if(pfunc->getTipo()->functionReturn() != CType::Void()) {
		if(!bReturnSaw)
			throw sem_exception(errFUNCTIONMUSTRETURNVALUE, pfunc->getNome());
		else if(!bReturned)
			throw sem_exception(errNOTALLCONTROLPATHSRETURN, pfunc->getNome());
	}

	// estrae l'attributo frameSize dal blocco
	currentFunction->frameSize = PBlock(pfunc->codice)->getFrameSize();
	currentFunction = 0;
	return p;
}

PNode Semantic::doRecurse(PNode p)
{
	process(p->left());
	return p;
}

//- BOOLEAN EXPRESSIONS: semantic
LabelSet Semantic::generaLabel()
{	
	return LabelSet(tmplabelCount++);
}

// converte il nodo ad un boolean!
PNode Semantic::doNcmp(PNode p)
{
	assert(*p >= Neq && *p <= Ngteq);
	CondType match[] = { cEQ, cNE, cLT, cGT, cLE, cGE};

	PBinExpr pe = p;
	exprproc(pe->sx());
	exprproc(pe->dx());
	pe->sx()->lvalue(false);
	pe->dx()->lvalue(false);
	TypeCheck::binary(*p, pe->sx()->ptipo, pe->dx()->ptipo);		

	// calcola il tipo del risultato
	pe->ptipo = pe->sx()->ptipo;		
	TypeCheck::convert(pe->dx(), pe->ptipo);

	// se entrambi costanti valuta e crea una boolexpr con 
	// descrittore costante
	if(pe->sx()->cvalue() && pe->dx()->cvalue()) {
		PBoolNode pbool = new CBoolNode(Nboolconst);
		pbool->ldesc.condizione = 
			CConstEvaluator::evalAsBool(
			  CConstEvaluator::eval(*p, pe->sx(), pe->dx())
			  ) ? cTRUE : cFALSE;
		delete pe;
		return pbool;
	}
	else {	
		CRelBoolExpr *pbool = new CRelBoolExpr(
			pe->sx(), pe->dx(), match[*p - Neq]);
		pe->sx() = PNode();
		pe->dx() = PNode();
		pbool->ptipo = pe->ptipo;
		delete pe;
		return pbool;		
	}

}

CExpression *Semantic::bool2expr(PNode p)
{
	assert(p->isBool());
	PBoolNode pbool = PBoolNode(p);
	
	if(pbool->isConst()) {
		int j = pbool->ldesc.condizione == cTRUE ? 1 : 0;
		delete pbool;
		return new CConstExpr(j);
	}
	else {
		if(pbool->ldesc.vero.none()) 
			pbool->ldesc.vero += generaLabel();
		if(pbool->ldesc.falso.none())
			pbool->ldesc.falso += generaLabel();
		CBool2Expr * pe = new CBool2Expr(pbool);
		pe->ptipo = CType::Int();	
		return pe;
	}
}

// processa l'espressione, se necessario genera una
// CExpr2Bool
PBoolNode Semantic::boolproc(PExpression pe)
{
	if(!pe) return PNode();
	assert(pe->isExpr());
	pe = process(pe);
	if(pe->isBool())
		return PBoolNode(pe);
	PBoolNode pbool = new CExpr2Bool(pe);
	pbool->ldesc.condizione = Cond(cNE);
	pbool->ptipo = pe->ptipo;
	return pbool;
}

// doNlogand
// Descrizione: trasforma (a and b) --> !(!a || !b)
PNode Semantic::doNlogand(PNode pp)
{
	assert(*pp == Nlogand);	
	PBinExpr p = pp;
	p->setNodeType(Nlogor);
	p->sx() = new CUnExpr(Nlognot, p->sx());
	p->dx() = new CUnExpr(Nlognot, p->dx());
	return process(new CUnExpr(Nlognot, PBinExpr(p)));	
}

// doNlnot
// Descrizione: nega il nodo
//		sx -> boolean pbool
//		pbool.not
//		ritorna pbool e distruggi this
PNode Semantic::doNlnot(PNode p)
{
	assert(*p == Nlognot);
	PUnExpr		pue(p);
	PBoolNode	pbool(boolproc(pue->sx()));	
	pbool->ldesc.not();
	pue->sx() = PNode();
	delete p;
	return pbool;
}

// doNlogor
// Descrizione: or bianrio, a or b --> (orelse1 a) orelse2 b
// viene spezzato in due sotto nodi
PNode Semantic::doNlogor(PNode p)
{
	assert(*p == Nlogor);
	PBoolExpr por1, por2;
	PBinExpr pe(p);

	por1 = new CBoolExpr(Norelse1, boolproc(pe->sx()));
	por1->ptipo = pe->sx()->ptipo;
	{ // or1 a
	LogicDesc & lda = por1->sx()->ldesc;
		if(lda == cFALSE) 
			por1->ldesc = LogicDesc(cNIL, lda.vero);			
		else
			por1->ldesc = LogicDesc(lda, lda.vero + generaLabel());
	}

	por2 = new CBoolExpr(Norelse2, por1, boolproc(pe->dx()));
	{ // c or1 b
	LogicDesc & ldb = por2->dx()->ldesc;
	LogicDesc & ldc = por2->sx()->ldesc;

		por2->ldesc = ldb == cFALSE ? 
			LogicDesc(cNIL, ldc.vero + ldb.vero, ldb.falso):
			LogicDesc(ldb, ldc.vero + ldb.vero, ldb.falso);
		por2->ptipo = pe->dx()->ptipo;
	}
	delete pe;
	return por2;
}


// Function name	: Semantic::doNexpr
// Description	    : 
// Return type		: PNode 
// Argument         : PNode p
//
// Nexpr( expr1 ) --> lineno = Nexpr.lineno; process expr1
PNode Semantic::doNexpr(PNode p)
{
	assert(p && p->isStat());
	setLineNo(PStatement(p)->lineNumber());
	try {
		process(p->left());
	}
	catch (sem_exception & e) {
		CMM::error(e.what());
	}
	return p;
}


// Function name	: Semantic::doNblock
// Description	    : 
// Return type		: PNode 
// Argument         : PNode p
//
// Nblock( stat list) --> table = Nblock.table; process each stat; 
//		sintesi dell'attributo frameSize; restore table
PNode Semantic::doNblock(PNode p)
{
	assert(p && *p == Nblock);
	PBlock pb = (PBlock)p;
	currentBlock = pb;
	CBlock::iterator itb = pb->begin();
	while(itb != pb->end()) {
		try {
			process(*itb);
		} catch(sem_exception & e) {
			CMM::error(e.what());
		}
		itb++;
	}

	// IL FRAME SIZE NON VA BENE PER AXP DIPENDE DAL CODEGEN
	// Scandisce la tabella dei simboli per calcolare frameSize
	// di questo blocco
	int k = pb->getFrameSize();
	FOR_EACH(psym, pb->getSymTab(), CSymbolTable::iterator, CSymbol*)
		if(!psym->isFunction() && psym->sType() == stoNORMAL) 
			k += TypeCheck::typeSize(psym->getTipo());
	END_EACH();
	pb->setFrameSize(k);

	// Scandisce tutti i sotto blocchi dei simboli per calcolare
	// il frameSize finale. Deve trovare il maggiore
	{
	int max = 0;
	CBlock::blkIterator it = pb->blkBegin();
	while(it != pb->blkEnd()) {
		if((*it)->getFrameSize() > max)
			max = (*it)->getFrameSize();
		it++;
	}
	pb->setFrameSize(pb->getFrameSize() + max);
	}

	// Ripristina il blocco del padre.
	currentBlock = pb->parent();	
	return p;
}

// ARITHMETIC EXPRESSIONS


// expr1 = expr2 è expr0
// Condizione: 
//   expr1.lvalue = true; 
//   expr2.tipo assignable a expr1.tipo; 
//   expr1.tipo non costante
// Azione controllo: 
//   expr0.lvalue = false; 
//   expr0.tipo   = expr1.tipo
// Azione traduzione: 
//   trad of expr2; 
//   rvalue(expr2); 
//   trad of expr1; 
//   store<tipo>
PNode Semantic::doNassign(PNode p)
{
	assert(p && *p == Nassign);
	PBinExpr pe(p);
	
	exprproc(pe->sx()); 
	exprproc(pe->dx());

	if(!pe->sx()->lvalue()) 
		throw operator_error(Nassign, errLVALUEREQUIRED);
	TypeCheck::assignable(pe->sx()->ptipo, pe->dx()->ptipo);
		
	pe->lvalue(false); 
	pe->ptipo = pe->sx()->ptipo;
	pe->dx()->lvalue(false);		
	TypeCheck::convert(pe->dx(), pe->ptipo);
	return pe;
}


PNode Semantic::doNpreincdec(PNode p)
{
	assert(p && (*p == Ninc || *p == Ndec));
	PUnExpr pe = p;
	exprproc(pe->sx());

	if(!pe->sx()->lvalue()) 
		throw operator_error(*p, errLVALUEREQUIRED);
	if(!pe->sx()->ptipo->isScalar())
		throw sem_exception(errOPERATOROVERLOADINGNOTIMPLEMENTED);
	
	pe->ptipo = pe->sx()->ptipo;
	pe->lvalue(false);
	return pe;

}

PNode Semantic::doNpostincdec(PNode) {return PNode();}
	
// & expr1 è expr0
// Condizione: expr1.lvalue = true
// Azione controllo: expr0.tipo = ptr(expr1.tipo);  expr0.lvalue = false
PNode Semantic::doNaddr(PNode p)
{
	assert(p && *p == Naddr);
	PUnExpr pe = p;

	exprproc(pe->sx());

	if(!pe->sx()->lvalue()) 
		throw operator_error(Naddr, errLVALUEREQUIRED);
	
	pe->ptipo = pe->sx()->ptipo->ptr();
	pe->lvalue(false);
	return pe;
}

// * expr1 è expr0
// Condizione: expr1.tipo = ptr
// Azione controllo: expr0.tipo = deref (expr1); expr0.lvalue = true
PNode Semantic::doNptr(PNode p)
{
	assert(p && *p == Nptr);
	PUnExpr pe = p;
	exprproc(pe->sx());

	if(!pe->sx()->ptipo->isPtr()) 
		throw sem_exception(errPTRREQUIRED);

	pe->ptipo = pe->sx()->ptipo->deref();
	pe->lvalue(true);		
	return pe;
}

//expr1 [ expr2 ] è expr0
// Condizione: expr1.tipo = ptr o array; expr2.tipo =integral; expr1.lvalue = true
//	vale anche l'inverso poiche' in C++ scrivere a[i] <> a+i <> i + a <> i[a]
// Azione controllo: expr0.tipo = deref(expr1 o 2); expr0.lvalue = true
PNode Semantic::doNarray(PNode p)
{
	assert(p && *p == Narray);
	PBinExpr pe = p;
	exprproc(pe->sx());
	exprproc(pe->dx());

	if(!(pe->sx()->ptipo->isArray() || pe->sx()->ptipo->isPtr())) 
		throw sem_exception(errSUBSCRIPTREQUIRESARRAY);	
	else if(!pe->dx()->ptipo->isIntegral()) 
		throw sem_exception(errSUBSCRIPTREQUIRESINTEGRAL);		
	else if(pe->sx()->ptipo->deref() == CType::Void()) 
		throw sem_exception(errCANNOTUSEVOIDPOINTER);		
	else {
		pe->ptipo = pe->sx()->ptipo->deref();
		TypeCheck::convert(pe->dx(), CType::Int());
		pe->lvalue(true);		
	}
	return pe;
}

/*
expr1 + expr2 è expr0
Condizione: possono valere le combinazioni di tipi (real, real), (integral, integral), (integral, real), (real, integra) oltre a due combinazioni speciali (ptr, int) e (int, ptr).
Azione controllo: expr0.lvalue = false
	(ptr, int)	expr0.tipo = expr1.tipo
	(int, ptr)	expr0.tipo = expr2.tipo
	(integral, integral) expr0.tipo = maxof(expr1.tipo, expr2.tipo)
	(real, real)	expr0.tipo = maxof(expr1.tipo, expr2.tipo)
	(real, integral) expr0.tipo = expr1.tipo
	(integral, real) expr0.tipo = expr2.tipo
Azione traduzione: 
	(ptr, int)	trad of expr1; rvalue(expr1); trad of expr2; rvalue(expr2); cvi
			const sizeof(deref(expr1)); muli; addp
	(int, ptr)	basta scambiare expr1 con expr2 nella precedente azione
	(others)	traduciamo il primo e poi il secondo, convertendoli al tipo giusto ed 
infine generando una add<maxtipo>
*/
PNode Semantic::doNadd(PNode p)
{
	assert(p && p->isExpr() && (*p == Nadd || *p == Nsub));
	PBinExpr pe = (PBinExpr)p;
	exprproc(pe->sx());
	exprproc(pe->dx());

	// commuta puntatori se add 
	if(*p == Nadd && (pe->dx()->ptipo->canbePtr() && pe->sx()->ptipo->isIntegral())) {
		PBinExpr tmp = pe->dx();
		pe->dx() = pe->sx();
		pe->sx() = tmp;
	}

	// pointer operation:
	//	 p - p
	//	 p + i
	//	 p - i
	//   i + p
	// where p cannot be void*
	// and i is an integral
	if(pe->sx()->ptipo->canbePtr())  {
		PType pointed = pe->sx()->ptipo->deref();
		int size = TypeCheck::typeSize(pointed);

		if(pointed == CType::Void()) throw sem_exception(errCANNOTUSEVOIDPOINTER);
				
		pe->sx()->lvalue(false);		
		
		if(pe->dx()->ptipo->isPtr()) {
			if(*p == Nadd) throw sem_exception(errPTRADDITIONNOTALLOWED);
			// ok e' una differenza di puntatori ma dello stesso sotto tipo
			if(pe->dx()->ptipo->deref() != pe->sx()->ptipo->deref())
				throw sem_exception(errCANNOTSUBDIFFPTR);
			pe->sx()->lvalue(false);
			pe->ptipo = CType::Int();
		}
		else if(pe->dx()->ptipo->isIntegral()) {
			TypeCheck::convert(pe->dx(), CType::Int());
			
			if(pe->dx()->cvalue()) 
				PConstExpr(pe->dx())->ival *= size;
			else if(size > 1) {
				PBinExpr pmul = new CBinExpr(Nmul, 
					pe->dx(), new CConstExpr(size));
				pe->dx() = pmul;
				exprproc(pe->dx());
				pe->dx()->lvalue(false);
			}
			pe->ptipo = pe->sx()->ptipo;
		}
		
		return pe;
		
	}
	else if (pe->dx()->ptipo->isPtr()) 
		throw sem_exception(errPOINTERADDSUBNOTIMPLEMENTED);			
	else if(pe->sx()->ptipo->isScalar() && pe->dx()->ptipo->isScalar()) {
		PType bestof = TypeCheck::bestType(pe->sx(), pe->dx());
		TypeCheck::convert(pe->sx(), bestof);
		TypeCheck::convert(pe->dx(), bestof);
		pe->sx()->lvalue(false);
		pe->dx()->lvalue(false);
		pe->lvalue(false);
		pe->ptipo = bestof;
		if(pe->sx()->cvalue() && pe->dx()->cvalue()) {
			PConstExpr pc = CConstEvaluator::eval(*p, pe->sx(), pe->dx());
			delete pe;
			return pc;
		}	
		else
			return pe;
	}
	else
		throw sem_exception(errOPERATOROVERLOADINGNOTIMPLEMENTED);		
	return PNode();
}


// expr1 * / % expr2 è expr0
// Condizione: 
// Azione controllo:
// Azione traduzione 
PNode Semantic::doNmul(PNode p) 
{
	assert(p && p->isExpr() && (*p == Nmod || *p == Nmul || *p == Ndiv));
	PBinExpr pe = (PBinExpr)p;	
	exprproc(pe->sx());
	exprproc(pe->dx());
	if(pe->sx()->ptipo->isScalar() && pe->dx()->ptipo->isScalar()) {
		PType bestof = TypeCheck::bestType(pe->sx(), pe->dx());
		TypeCheck::convert(pe->sx(), bestof);
		TypeCheck::convert(pe->dx(), bestof);		
		pe->sx()->lvalue(false); pe->dx()->lvalue(false);
		pe->lvalue(false);		
		pe->ptipo = bestof;
		if(pe->sx()->cvalue() && pe->dx()->cvalue()) {
			PConstExpr pc = CConstEvaluator::eval(*p, pe->sx(), pe->dx());
			delete pe;
			return pc;
		}	
		else
			return pe;
	}
	else {
		throw sem_exception(errOPERATOROVERLOADINGNOTIMPLEMENTED);
		return PNode();
	}
}

// Binary Bit Operation: & | ^
// expr1 | & ^ expr2 --> expr0
// Condizione: expr1, expr2 devono essre integral
// Azione di controllo: expr0.lvalue = false, expr0.tipo = maxof(expr1.tipo, expr2.tipo)
// Azione di traduzione: trad of expr1; rvalue(expr1); trad of expr2; rvalue(expr2); operazione 
PNode Semantic::doNbinop(PNode p) 
{
	assert(p && p->isExpr() && (*p == Nbinor || *p == Nbinand || *p == Nbinxor
		|| *p == Nshl || *p == Nshr));
	PBinExpr pe = (PBinExpr)p;	
	exprproc(pe->sx());
	exprproc(pe->dx());
	if(pe->sx()->ptipo->isIntegral() && pe->dx()->ptipo->isIntegral()) {
		PType bestof = TypeCheck::bestType(pe->sx(), pe->dx());
		TypeCheck::convert(pe->sx(), bestof);
		TypeCheck::convert(pe->dx(), bestof);		
		pe->sx()->lvalue(false); pe->dx()->lvalue(false);
		pe->lvalue(false);
		if(pe->sx()->cvalue() && pe->dx()->cvalue()) {
			PConstExpr pc = CConstEvaluator::eval(*p, pe->sx(), pe->dx());
			delete pe;
			return pc;
		}	
		else
			return pe;		
	}
	else {
		throw operator_error(*p, errINTEGRALREQUIRED);
		return PNode();
	}
	
}
 
// binnot expr1 --> expr0
// Condizione: expr1 sia integral
// Azione di controllo: expr0.lvalue = false; expr0.tipo = expr1.tipo
// Azione di traduzione: trad of expr1; rvalue(expr1); not<tipo>
PNode Semantic::doNbinnot(PNode p)
{
	assert(p && p->isExpr() && *p == Nbinnot);
	PUnExpr pe = (PUnExpr)p;
	exprproc(pe->sx());
	if(pe->sx()->ptipo->isIntegral()) {
		pe->ptipo = pe->sx()->ptipo;
		pe->sx()->lvalue(false);
		pe->lvalue(false); 
		if(pe->sx()->cvalue()) {
			PConstExpr pc = CConstEvaluator::eval(*p, pe->sx());
			delete pe;
			return pc;
		}
		else
			return pe;
	}
	else {		
		throw operator_error(*p, errINTEGRALREQUIRED);
		return PNode();
	}

}

// - expr1 è expr0
// Condizione: expr1 sia scalare
// Azione di controllo: expr0.lvalue = false; expr0.tipo = expr1;
// Azione di traduzione: trad of expr1; rvalue(expr1); neg<tipo>
PNode Semantic::doNneg(PNode p)
{
	assert(p && p->isExpr() && *p == Nneg);
	PUnExpr pe = (PUnExpr)p;
	exprproc(pe->sx());
	if(pe->sx()->ptipo->isScalar()) {
		pe->ptipo = pe->sx()->ptipo;
		pe->sx()->lvalue(false);
		pe->lvalue(false); 
		if(pe->sx()->cvalue()) {
			PConstExpr pc = CConstEvaluator::eval(*p, pe->sx());
			
			delete pe;
			return pc;
		}
		else
			return pe;		
	}
	else {
		throw operator_error(*p, errSCALARREQUIRED);
		return PNode();
	}

}


/*
The comma operator, also called the "sequential-evaluation" operator, 
evaluates its two operands sequentially from left to right. 

The left operand of the comma operator is evaluated as a void 
 expression. The result of the operation has the same value and type
 as the right operand. Each operand can be of any type. The comma
 operator does not perform type conversions between its operands,
 and it does not yield an l-value. There is a sequence point after
 the first operand, which means all side effects from the evaluation
 of the left operand are completed before beginning evaluation of 
 the right operand. 

*/
PNode Semantic::doNcomma(PNode p) 
{
	assert(p && p->isExpr() && (*p == Ncomma));
	PComma pe = (PComma)p;
	CComma::iterator it = pe->begin();
	int n = pe->conta();
	for(int i = 0; i < n-1 ;i++, it++){
		exprproc(*it);				
		TypeCheck::convert(*it, CType::Void(), true);
	}
	exprproc(*it);
	(*it)->lvalue(false);
	pe->lvalue(false); 
	// pe->cvalue(false); 
	pe->ptipo = (*it)->ptipo;
	return pe;
}

PNode Semantic::doNcast(PNode p)
{
	assert(p && p->isExpr() && (*p == Ncast));
	PUnExpr pe = (PUnExpr)p;
	exprproc(pe->sx());
	if(TypeCheck::assignable(pe->ptipo, pe->sx()->ptipo)) {
		TypeCheck::convert(pe->sx(), pe->ptipo);
		pe->sx()->lvalue(false);
		PNode pn = pe->sx();
		pe->sx() = PNode();
		delete pe;
		return pn;
	}
	else {
		throw sem_exception(errCANNOTDOTYPECAST);
		return PNode();
	}	
}

//- STATEMENTS SEMANTIC ANALISYS

/*	
	jcond (not c.cond), L1
	label c.vero
	body dell'if
	[ jmp L2 se c'e' else ]
	L1:
	c = < c.cond, *, c.falso U (L1), *>
	[ 
	body else
	]
	L2:

  
*/
// bReturned = bReturnedBefore || (bIf && bElse);
PNode Semantic::doNif(PNode p)
{
	assert(*p == Nif && p->isStat());
	PBoolNode pbool;
	PCondStat ps = PCondStat(p);
	bool bretBefore,bretIf;
	
	ps    = PCondStat(p);
	pbool = boolproc(ps->cond);
	if(pbool->ldesc.falso.none())
		pbool->ldesc.falso += generaLabel();
	
	ps->bcond = pbool;

	bretBefore = bReturned;
	process(ps->block);
	bretIf = bReturned;
	if(!bretBefore) bReturned = false;

	if(ps->blockII) {
		process(ps->blockII);
		// in questo caso genera anche unisci
		pbool->ldesc.unisci += generaLabel();				
	}		
	else
		bReturned = false;
	bReturned = bretBefore || (bretIf && bReturned);
	
	return p;
}

  
// preserve bReturned
PNode Semantic::doNwhile(PNode p)
{
	assert((*p == Nwhile || *p == Ndo)&& p->isStat());
	PBoolNode pbool;
	PCondStat ps;	

	ps    = PCondStat(p);
	pbool = boolproc(ps->cond);
	pbool->ldesc.vero += generaLabel();
	pbool->ldesc.falso += generaLabel();
	pbool->ldesc.unisci += generaLabel();
	ps->bcond = pbool;

	bool bRet = bReturned;
	process(ps->block);
	bReturned = bRet;
	return p;
}

PNode Semantic::doNfor(PNode p)
{
	assert(*p == Nfor && p->isStat());
	PBoolNode pbool;
	PCondStat ps;

	ps = PCondStat();
	if(ps->block) process(ps->block);
	pbool = boolproc(ps->cond);
	pbool->ldesc.falso += generaLabel();
	pbool->ldesc.unisci += generaLabel();
	ps->bcond = pbool;
	if(ps->incr) process(ps->incr);
	
	bool bRet = bReturned;		
	if(ps->blockII) process(ps->blockII);
	bReturned = bRet;
	return ps;
}

// Function: doNreturn
// Description: processa l'istruzione return
//   valuta espressione
//	 check: assegnabile(return type, expression type)
PNode Semantic::doNreturn(PNode p)
{
	assert(p && p->isStat() && (*p == Nreturn));	
	PExprStat pe = p;	

	setLineNo(pe->lineNumber());	
	try {
		exprproc(pe->expr);		
	}
	catch (sem_exception) {
		return p;
	}

	try {
		PType rt = currentFunction->getTipo()->functionReturn();
		if(rt == CType::Void() && pe->expr->ptipo != rt)
			throw sem_exception(errVOIDRETURNING);
		TypeCheck::assignable(currentFunction->getTipo()->functionReturn(), pe->expr->ptipo);				
		pe->expr->lvalue(false);		
		TypeCheck::convert(pe->expr, currentFunction->getTipo()->functionReturn(),true);			

		bReturned = bReturnSaw = true;
	}
	catch(sem_exception &e) {		
		CMM::error(e.what());
	}	
	
	return p;
}

// Funzione: doNcall
// Descrizione: chiamata a funzione
//	
PNode Semantic::doNcall(PNode p)
{
	assert(p && *p == Ncall && p->isExpr());

	PBinExpr pe = p;
	PType proto;
	
	exprproc(pe->sx());
	if(pe->sx()->ptipo->isPtr()) 
		proto = pe->sx()->ptipo->deref();
	else
		proto = pe->sx()->ptipo;
	if(!proto->isFunc()) 
		throw sem_exception(errCALLREQUIRESFUNCTIONOPERAND);
		

	// Argument Processing
	PComma pc = (PComma)pe->dx();
	if(pc) {
		CComma::iterator it = pc->begin();
		int n = pc->conta();
		int baseargs = proto->getParamCount();
		bool bEllipse = tELLIPSE == proto->getParameter(baseargs-1)->getType();
		if(bEllipse) baseargs--;
		
		// troppi parametri e non ellipse function allora errore!
		if(bEllipse) {
			if(n < baseargs)
				throw sem_exception(errTOOLESSARGUMENTS);
		}
		else {
			if(n > baseargs) 
				throw sem_exception(errTOOMUCHARGUMENTS);		
			else if(n < baseargs) 
				throw sem_exception(errTOOLESSARGUMENTS);
		}

		for(int i = 0; i < n;i++, it++){
			exprproc(*it);
			if(i < baseargs) {
				try {
					TypeCheck::assignable((*it)->ptipo, proto->getParameter(i));
					(*it)->lvalue(false);
					TypeCheck::convert(*it, proto->getParameter(i), true);
				}	
				catch (sem_exception)  {
					throw cannot_convert_error(errARGUMENTTYPEMISMATCH,
						i+1, (*it)->ptipo, proto->getParameter(i));
				}
			}		
			else {
				assert(bEllipse);
				(*it)->lvalue(false);
				PType argtype = (*it)->ptipo;
				if(argtype == CType::Void() )
					throw sem_exception(errILLEGALVOID);
			}

		}
	}
	else if (proto->getParamCount() > 0)
		throw sem_exception(errTOOLESSARGUMENTS);
	pe->lvalue(false); 
	pe->ptipo = proto->functionReturn();

	if(currentFunction) 
		currentFunction->functionHasCalledSomeone();
	return pe;

}




// TYPE CHECKING

// Function name	: TypeCheck::convert
// Description	    : 
// Return type		: void 
// Argument         : PExpression & srcExpr
// Argument         : PType toType
//
// Converte l'espressione srcExpr in una nuova espressione
// per la conversione di tipo usiamo il nodo Ncast (CUnExpr)
// che definisce il tipo risultante.
// LVALUE = se conversione fra puntatori mantengo lvalue, altr. false
void TypeCheck::convert(PExpression & srcExpr, PType toType, bool forced)
{
	assert(srcExpr && srcExpr->isExpr());
	PType fromType = srcExpr->ptipo;
	if(fromType == toType) return;

	// ptr, ptr: accetta solo se forced
	if((fromType->isPtr() || fromType->isArray()) && toType->isPtr()) {
		if(!forced) 
			throw sem_exception(errPTRCASTERROR);
		else {
			srcExpr->ptipo = toType;			
		}
		return;
	}
	else if(toType->isArray() || toType->isFunc()) 
		throw sem_exception(errCANNOTCASTTOARRAYORFUNCTION);
	else {
		if(srcExpr->cvalue()) 
			CConstEvaluator::cast(srcExpr, toType);		
		else {
			srcExpr = new CUnExpr(Ncast, srcExpr);
			srcExpr->ptipo = toType;
		}

	}
}


// Function name	: TypeCheck::bestType
// Description	    : 
// Return type		: PType 
// Argument         : PExpression p1
// Argument         : PExpression p2
PType TypeCheck::bestType(PExpression p1, PExpression p2)
{
	assert(p1 && p2 && p1->isExpr() && p2->isExpr());
	PType t1, t2;
	t1 = p1->ptipo;
	t2 = p2->ptipo;
	assert(t1->isScalar() && t2->isScalar());

	if(t1 == t2) return t1;
	if(t1->isFloat() || t2->isFloat()) return CType::Float();
	if(t1->isInt() || t2->isInt()) return CType::Int();
	if(t1->getType() == tCHAR || 
		t2->getType() == tCHAR) return CType::Char();
	assert(0);		// NEVER REACH HERE
	return 0;
}


// Function name	: TypeCheck::assignable
// Description	    : 
// Return type		: puntatore alla funzione dell'assegnamento o 0
// Argument         : PType target
// Argument         : PType source
//
// 
PFunction TypeCheck::assignable(PType dx, PType sx)
{
	assert(dx && sx);
	if(dx== CType::Void() || sx == CType::Void())
		throw sem_exception(errILLEGALVOID);

	if(sx->isScalar() && dx->isScalar()) return 0;

	PType p1, p2;
	
	p1 = sx->isArray() ? sx->arrayToPointer() : sx;
	p2 = dx->isArray() ? dx->arrayToPointer() : dx;
	
	if(p1->isPtr() && p2->isPtr()) {
		if(p1->deref() != p2->deref()) 
			throw cannot_convert_error(errCANNOTCONVERT_PTR,Nassign, sx, dx);
		else
			return 0;
	}
	else 
		throw cannot_convert_error(errCANNOTCONVERT, Nassign, sx, dx);	
	return 0;
}

// binary
// Description: controlla che esista un operatore nt tra source e
//   target, se l'operatore non esiste genera un'eccezione, 
//	 se invece esiste e non e' l'operazione di base ritorna la funzione
//	 altrimenti 0
// NOTA: non gestiamo i puntatori a funzione ne' le strutture dunque
// la funzione tornera' sempre 0
// Semantica:
//		basetype		 op basetype	OK
//		ptr/array		 op	ptr/array	OK <==> stesso sottotipo
PFunction TypeCheck::binary(NodeType nt, PType sx, PType dx)
{
	assert(sx && dx);
	if(sx == CType::Void() || dx == CType::Void())
		throw sem_exception(errILLEGALVOID);

	if(sx->isScalar() && dx->isScalar()) return 0;

	PType p1, p2;
	
	p1 = sx->isArray() ? sx->arrayToPointer() : sx;
	p2 = dx->isArray() ? dx->arrayToPointer() : dx;
	
	if(p1->isPtr() && p2->isPtr()) {
		if(p1->deref() != p2->deref()) 
			throw cannot_convert_error(errCANNOTCONVERT_PTR,nt, sx, dx);
		else
			return 0;
	}
	else 
		throw cannot_convert_error(errCANNOTCONVERT, nt, sx, dx);	
	return 0;
}

bool TypeCheck::x86target;

int TypeCheck::typeSize(PType t)
{
	if(x86target) {
		switch(t->getType()) {
		case tCHAR: return 1;	// used for alignment
		case tINT: case tFLOAT: case tPTR: return 4;
		case tARRAY:
			return t->getArraySize() * typeSize(t->deref());
		case tVOID: return 0;
		case tFUNC: return 4;	// as a pointer
		case tSTRUCT:
			//return t->getSymbol()->size();
			return 0;
		default:
			cout << "ERRORE INTERNO TypeCheck::typeSize\n";
			return 0;
		}
	}
	else {
		switch(t->getType()) {
		case tCHAR: return 1;	// used for alignment
		case tINT: case tFLOAT: return 4;
		case tPTR: return 8;
		case tARRAY:
			return t->getArraySize() * typeSize(t->deref());
		case tVOID: return 0;
		case tFUNC: return 8;	// as a pointer
		case tSTRUCT:
			//return t->getSymbol()->size();
			return 0;
		default:
			assert(0); // cout << "ERRORE INTERNO TypeCheck::typeSize\n";
			return 0;
		}
	}
}

PConstExpr CConstEvaluator::eval(NodeType op, PExpression & s1, PExpression & s2)
{
	PConstExpr ret = s1;
	// la divisione delle operazioni avviene in base al tipo
	// e poi in base ad op
	if(s1->ptipo == CType::Char()) {
		char & c2 = ((PConstExpr)s2)->cval;
		char & c1 = ret->cval;		
		switch(op) {
		case Nadd: c1 += c2; break;
		case Nmul: c1 *= c2; break;
		case Ndiv: c1 /= c2; break;
		case Nsub: c1 -= c2; break;
		case Nmod: c1 %= c2; break;
		case Nbinor: c1 |= c2; break;
		case Nbinand: c1 |= c2; break;
		case Nbinxor: c1 |= c2; break;
		case Nshl: c1 <<= c2; break;
		case Nshr: c1 >>= c2; break;
		// case Neq: c1 
		}
	}
	else if(s1->ptipo == CType::Float()) {
		float & c2 = ((PConstExpr)s2)->fval;
		float & c1 = ret->fval;		
		switch(op) {
		case Nadd: c1 += c2; break;
		case Nmul: c1 *= c2; break;
		case Ndiv: c1 /= c2; break;
		case Nsub: c1 -= c2; break;
		}

	}
	else if(s1->ptipo == CType::Int() || s1->ptipo->isPtr()) {
		int & c2 = ((PConstExpr)s2)->ival;
		int & c1 = ret->ival;		
		switch(op) {
		case Nadd: c1 += c2; break;
		case Nmul: c1 *= c2; break;
		case Ndiv: c1 /= c2; break;
		case Nsub: c1 -= c2; break;
		case Nmod: c1 %= c2; break;
		case Nbinor: c1 |= c2; break;
		case Nbinand: c1 |= c2; break;
		case Nbinxor: c1 |= c2; break;
		case Nshl: c1 <<= c2; break;
		case Nshr: c1 >>= c2; break;
		// case Neq: c1 
		}
	}
	
	s2 = PNode();
	s1 = PNode();
	delete s2;
	return ret;
}

PConstExpr CConstEvaluator::eval(NodeType op, PExpression & s1)
{
	PConstExpr ret = s1;
	if(s1->ptipo == CType::Int()) {
		int r = ret->ival;
		switch(op) {
		case Nneg: r = -r; break;
		};
		ret->ival = r;		
	}
	else if(s1->ptipo == CType::Char()) {
		char r = ret->cval;
		switch(op) {
		case Nneg: r = -r; break;
		};
		ret->cval = r;		
	
	}
	else if(s1->ptipo == CType::Float()) {
		float r = ret->fval;
		switch(op) {
		case Nneg: r = -r; break;
		};
		ret->fval = r;		
	
	}
	return ret;
}

PConstExpr CConstEvaluator::cast(PExpression & s1, PType tipo)
{
	PConstExpr ret = s1;
	if(ret->ptipo == tipo) return ret;
	if(ret->ptipo == CType::Char()) {
		char  v = ret->cval;		
		switch(tipo->getType()) {
		case tPTR:
		case tLONG:
		case tINT: ret->ival = v;   break;
		case tFLOAT: ret->fval = v; break;
		case tVOID: break;
		default: break;
		}
	}
	else if(ret->ptipo == CType::Float()) {
		float  v = ret->fval;		
		switch(tipo->getType()) {
		case tPTR:
		case tLONG:
		case tINT: ret->ival = int(v);   break;
		case tCHAR: ret->cval = int(v); break;
		case tVOID: break;
		default: break;
		}		
	}
	else if(s1->ptipo == CType::Int() || s1->ptipo->isPtr()) {
		int  v = ret->ival;		
		switch(tipo->getType()) {
		case tCHAR: ret->cval = v; break;
		case tFLOAT: ret->fval = float(v); break;
		case tVOID: break;
		default: break;
		}		
	}
	ret->ptipo = tipo;
	return ret;
}

bool CConstEvaluator::evalAsBool(PConstExpr)
{
	return false;
}
