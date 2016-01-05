// Nome: axpgen
// Descrizione: generator ecodice Alpha
// Autore: Ruffaldi Emanuele
// Progetto: CPP/codegen
#include "axpgen.h"
#include <string.h>

 template <class T>
T max(T t1, T t2)
{return t1 > t2 ? t1 : t2;}

 template <class T>
T min(T t1, T t2)
{return t1 < t2 ? t1 : t2;}


static CSymbol * asSymbol(CExpression * pe)
{
	if(*pe != Nsymbol) return 0;
	return PSymExpr(pe)->getSymbol();
}


static int align(int & k, int v = 3)
{
	k = (k+v) & ~v;
	return k;
}

// Stringhe per l'inizio e la fine del file
static const char * prefix = 
	".set noreorder\n"".set volatile\n" ".set noat\n"
	"#include<regdef.h>\n";
static const char * postfix = "";


AXPGen::AXPGen(char * source, char *output, PBlock pb) :
	out(output) , obuf(outbuf, sizeof(outbuf)), ocomment(comment, sizeof(comment)),
		inCode(false), ftoldeclared(false), bool2exprlabelcount(0),
		exitlabelcount(0)
{		
	if(!out) return;

	preCode();

	// name mangling e definizione liste GLOBAL, STATIC, EXTERN  
	processSymbols(pb, 0);	
	// dichiarazioni di visibilita' per le variabili globali non statiche	
	printPublicVar();
	// definizione ed init delle variabili globali non statiche
	if(!globalVar.empty()) 	definePublicVar();		
	// dichiarazione per le funzioni globali non statiche ne' extern	
	printPublicFun();
	// dichiara le funzioni e le variabili esterne USATE
	printExternSym();
	// output delle costanti
	
	// itera su tutte le funzioni 
	processFunctions();
	ofunctionCode << std::ends;
	out << ofunctionCode.str();
	postCode();
}

bool AXPGen::ready() {return !out.fail();}

// Function name	: AXPGen::preCode
// Description	    : output the fixed part of the file
// Return type		: void 
void AXPGen::preCode()
{
	out << prefix;
}

void AXPGen::postCode()
{
	out << postfix;
}
	
char * AXPGen::mangleSymbol(CSymbol *ps)
{
	return ps->asmName;
}

void AXPGen::addComment(char * s)
{
	ocomment <<"//" << s;	
}

void AXPGen::newline()
{
	int n = obuf.pcount();
	if(n < 40) {
		for(int i = 0; i < 40-n; i++)
			obuf << ' ';
	}
	ocomment << std::ends;
	obuf << ocomment.str() << std::ends;	
	ofunctionCode << (inCode ? "\t\t" : "") << obuf.str() << std::endl;
	obuf.seekp(0);
	ocomment.seekp(0);
}

// Function name	: AXPGen::printPublicVars
// Description	    : stampa le dichiarazioni public per le funzioni
//						globali non statiche
// Return type		: void 
void AXPGen::printPublicFun()
{
	return ;	
}

// Function name	: AXPGen::printPublicVars
// Description	    : stampa le dichiarazioni extern
// Return type		: void 
void AXPGen::printExternSym()
{
	slista<CSymbol*>::iterator it = symEsterni.begin();
	while(it != symEsterni.end()) {
		CSymbol *psym = *it;		
		if(!psym->isFunction()) {
			obuf << ".extern " << mangleSymbol(psym) << " ";
			obuf << fullSizeOf(psym->getTipo());
			addComment(psym->getNome());
			newline();		
		}
        ++it;
	}
}


// Function name	: AXPGen::printPublicVars
// Description	    : stampa le dichiarazioni public per le variabili
//						globali non statiche
// Return type		: void 
void AXPGen::printPublicVar()
{
	slista<CSymbol *>::iterator it = globalVar.begin();
	obuf << ".data" ; newline();
	while(it != globalVar.end()) {
		CSymbol *psym = *it;
		obuf << ".globl " << mangleSymbol(psym);
		//addComment(psym->getNome());
		newline();
        ++it;
	}
}

void AXPGen::definePublicVar()
{
	slista<CSymbol *>::iterator it = globalVar.begin();	
	obuf << ".data" ; newline();
	while(it != globalVar.end()) {
		CSymbol *psym = *it;

		// no inizialization yet
		obuf << ".align 3" ; newline();
		obuf << mangleSymbol(psym) << ":" ; newline();
		obuf << ".space " << fullSizeOf(psym->getTipo());
	    newline();
        ++it;
	}	
}

// where = 0 root, 1 funzione, 2 blocco
// * Calcola asmName per ogni simbolo
// * Classifica: static / extern / globalVar
void AXPGen::processSymbols(PBlock pb, int where)
{
    FOR_EACH(psym, pb->getSymTab(), CSymbolTable::iterator, CSymbol*)
    	std::ostrstream ost;
		if(psym->isFunction()) {
			if(psym->isExtern()) {
				if(psym->used)
					symEsterni.push_back(psym);
			}
			else 
				funDefinite.push_back((CFunction*)psym);						
			ost << psym->getNome() << std::ends;
			psym->asmName = ost.str();
		}
		// variabili globali
		else if(where == 0) {
			if(psym->sType() == stoNORMAL)
				globalVar.push_back(psym);
			else if(psym->sType() == stoEXTERN)
				symEsterni.push_back(psym);		
			ost << psym->getNome() << std::ends;	
			psym->asmName = ost.str();
		}	
		// simboli locali ad una funzione
		else {			
			if(psym->sType() == stoEXTERN) 
				symEsterni.push_back(psym);			
			// calcola mangled name come in processLocalBlock
			std::ostrstream ost;
			ost << psym->getNome() << "$";
			if(where == 2)
				ost << std::dec << pb->id();
			ost << std::ends;
			psym->asmName = ost.str();

		}        
    END_EACH()

	// Analizza tutti i sotto blocchi
	CBlock::blkIterator it2 = pb->blkBegin();
    while(it2 != pb->blkEnd()) {
        processSymbols(PBlock(*it2), 
			where == 0 ? 1 : 2);
        ++it2;
	}
}


// Function name	: AXPGen::asmtypeof
// Description	    : ritorna il tipo assembler per un item x
// Return type		: AsmType 
// Argument         : PType
AXPGen::AsmType AXPGen::asmTypeOf(PType t)
{
	while(t->isArray()) 
		t = t->deref();
	switch(t->getType()) {
	case tINT: return AINT;
	case tCHAR: return ACHAR;
	case tPTR: return APTR;
	case tFLOAT: return AFLOAT;
	case tVOID: return AVOID;
	default:
		std::cout << "INTERNAL ERROR IN asmtypeof\n";
		return AVOID;
	}
}

void AXPGen::processFunctions()
{
	slista<CFunction *>::iterator it = funDefinite.begin();
	ofunctionCode << ".text\n";	
	while(it != funDefinite.end()) {
		CFunction *psym = *it;
		if(psym->inited) {
			// processa il blocco della funzione
			exitlabelcount++;
			prolog(psym);			
			gen(PNode(psym->getBody()));			
			epilog(psym);			
			newline();
		}
        ++it;
	}
	// ofunctionCode << "\n";
}

int AXPGen::getStackRequest(PNode p)
{

	if(!p) return 0;
	switch(*p) {
		case Nconst:  return 1;
		case Nsymbol: return 1;
		case Ndeclstat: return 0;
		
		// binary
		case Nadd:
		case Nsub:	
		case Nmul: 
		case Ndiv: 
		case Nmod: 
		case Nbinand:
		case Nbinor:
		case Nbinxor:  
		case Nshl:
		case Nshr:	
		case Ncmp: 
		case Norelse2:
		case Nassign:
			{ int t1 = getStackRequest(p->left());
			  int t2 = getStackRequest(p->right());
			  if(t1 == t2) return t1 +1;
			  else return max(t1, t2);
			}			
			break;
		// unary
		case Ncast:
		case Nreturn:
		case Naddr: 
		case Nneg:
		case Nbinnot:
		case Nbool2expr:
		case Nexpr2bool:
		case Norelse1:
		case Nexpr: 
			return getStackRequest(p->left()); 

		// multiple
		case Nblock: 
			{
				int k = 0;
				CBlock::iterator it = ((PBlock)p)->begin();
				while(it != ((PBlock)p)->end()) {
					k = max(k, getStackRequest(*it));
					++it;
				}
				return k;
			}
		case Ncomma:    
			{
				PComma pc = p->left();
				CComma::iterator it = pc->begin();
				int k = 0;
				for(;it != pc->end();it++) 
					k = max(k, getStackRequest(*it));
				return k  + pc->conta();				
			}
					
		case Ncall:	
			{
			PBinExpr pp = (PBinExpr)p;
			PComma pc = (PComma)pp->left();

				CComma::iterator it = pc->begin();

				int k = 0;
				int i = 0;
				// nel passaggio ad una funzione il mio 
				for(;it != pc->end();i++, it++) 
					k = max(k, getStackRequest(*it));
				return 1+k  +i;				
			}			
		// statements = 
		case Nif:
		case Nwhile:
		case Ndo:
			{
			int k = max(getStackRequest(p->left()), getStackRequest(p->right()));
				k = max(k, getStackRequest(PCondStat(p)->blockII));
				return k;
			}
		case Nfor:
			{
			int k = max(getStackRequest(p->left()), getStackRequest(p->right()));
				k = max(k, getStackRequest(PCondStat(p)->blockII));
				k = max(k, getStackRequest(PCondStat(p)->incr));
				return k;
			}			
		default:
			break;
	}
	return 0;
}


void AXPGen::prolog(CFunction *psym)
{
	frame.cntStackSize = getStackRequest(PNode(psym->getBody()))*8;
	frame.size = frame.cntStackSize + 24;
	frame.offLocalVar = frame.size;
	int k = frame.offLocalVar;
	processLocalBlock(PBlock(psym->codice), 
		k, true);
	frame.cntLocalVar = k - frame.offLocalVar;
	
	frame.size += frame.cntLocalVar;
	align(frame.size, 7);
	frame.offSavedParams = frame.size;
	frame.cntSavedParams = min(6, psym->getTipo()->getParamCount())*8;
	frame.size += frame.cntSavedParams;
	frame.extraParams = frame.size;
	
	processLocalSymbols(psym);
	
	ofunctionCode << ".align 3\n";	
	ofunctionCode << ".globl " << mangleSymbol(psym) << std::endl;
	ofunctionCode << ".ent " << mangleSymbol(psym) << std::endl;
	ofunctionCode << mangleSymbol(psym) << ":" << std::endl;

	int argcount =  psym->getTipo()->getParamCount();
	int kmax = argcount > 6 ? 6: argcount;	
	

	inCode = true;
	emitIstr("ldgp", rGP, memory(rPV,0)); newline();
	emitIstr("lda", rSP, memory(rSP, -frame.size)); newline();
	obuf << ".frame fp, " << frame.size << ", ra , 0"; newline();
	emitIstr("stq", rRA, memory(rSP,frame.cntStackSize)); newline();
	emitIstr("stq", rFP, memory(rSP,frame.cntStackSize+8)); newline();
	emitIstr("stq", rS0, memory(rSP,frame.cntStackSize+16)); newline();
	emitIstr("bis", rSP, rSP, rFP); newline();
	obuf << ".prologue 1"; newline();

	// se la funzione non e' una foglia allora salvo i 
	// registri a0.. dalla posizione FIRSTLOCALARG
	for(int i = 0; i < kmax; i++) {
		AsmType t = asmTypeOf(psym->getTipo()->getParameter(i));
		emitIstr(makeTypedIstr("st",t),
			(t == AFLOAT ? 16  : rA0) + i, 
			memory(rFP, frame.offSavedParams + i*8), t == AFLOAT);
		newline();
	}
	
	emitIstr("lda", rS0, memory(rSP,frame.cntStackSize)); newline();	
	
}

void AXPGen::epilog(CFunction *psym)
{
	obuf << "$" << exitlabelcount << "kkk:"; newline();
	emitIstr("bis", rSP, rSP, rFP); newline();
	emitIstr("ldq", rS0, memory(rSP, frame.cntStackSize+16)); newline();
	emitIstr("ldq", rRA, memory(rSP, frame.cntStackSize+0)); newline();
	emitIstr("ldq", rFP, memory(rSP, frame.cntStackSize+8)); newline();
	emitIstr("addq", rSP, imm(frame.size), rSP); newline();
	obuf << "ret $31, (ra), 1"; newline();
	inCode = false;
	ofunctionCode << ".end " << mangleSymbol(psym) << std::endl;
}

int AXPGen::fullSizeOf(PType t)
{
	int k = 1;
	while(t->isArray()) {
		k *= t->getArraySize();
		t = t->deref();
	}
	switch(t->getType()) {
	case tINT: case tFLOAT: return k* 4;
	case tPTR: case tFUNC: return k*8;
	case tCHAR: return k;
	default: return 0;
	}

}

int AXPGen::argSizeOf(PType)
{
	return 8;
}

void AXPGen::emitSymOffset(CSymbol* psym)
{
	ofunctionCode << mangleSymbol(psym);
	ofunctionCode << " = " << std::dec << (psym->offset) << "\n";
}



// Function name	: AXPGen::processLocalSymbols
// Description	    : calcola offset delle variabili locali
//					  e calcola/stampa i nomi dei simboli locali
// Return type		: void 
// Argument         : CFunction *pfunc
// 
// calcola gli offset delle variabili locali, 
// _ nome $ codiceblocco	=	offset sullo stack
void AXPGen::processLocalSymbols(CFunction *pfunc)
{
	
	CSymbolTable::iterator it = pfunc->getTable().begin();
    CSymbolTable::iterator ite = pfunc->getTable().end();
	
    CSymbol * * parameters = new PSymbol[pfunc->getTipo()->getParamCount()];
	for(;it != ite; ++it ) {
        CSymbol * psym = *it;
		if(psym->isParam()) {
			parameters[psym->index] = psym;
		}
	}
	
	int offExtra = frame.extraParams;
	int offSaved = frame.offSavedParams;
	for(int i = 0; i < pfunc->getTipo()->getParamCount(); i++) {
		CSymbol * psym = parameters[i];
		if(i >= 6 ) {
			psym->offset = align(offExtra,7);				
			offExtra += argSizeOf(psym->getTipo());
					
		}
		else if(i < 6) {
			psym->offset = offSaved;
			offSaved += 8;				
		}
		std::cout << "Adjusted offset of " << psym->getNome() <<
			 " = " << psym->offset << std::endl;
	}        
	delete [] parameters;
	
}


// Function name	: AXPGen::processLocalBlock
// Description	    : 
// Return type		: void 
// Argument         : PBlock pb
// Argument         : int 
// Argument         : bool topmost = false
//
// Calcola gli spiazzamenti delle variabili locali ad una
// funzione. Ogni variabile viene allineata ai 4byte
// se non e' un char.
void AXPGen::processLocalBlock(PBlock pb, int &offset , bool topmost)
{
    FOR_EACH(psym, pb->getSymTab(), CSymbolTable::iterator, CSymbol*)
    	if(psym->sType() == stoNORMAL) {			
			if(psym->getTipo() != CType::Char())
				psym->offset = align(offset);
			else 
				psym->offset = offset;
			offset += fullSizeOf(psym->getTipo());			
		}        
	END_EACH();

	// Analizza tutti i sotto blocchi	
	CBlock::blkIterator itb = pb->blkBegin();
	while(itb != pb->blkEnd()) {
        int k = offset;
		processLocalBlock(PBlock(*itb), k, false);
		if(k > offset) offset = k;
		++itb;
	}
	
}


// CODE GENERATIONS: by node
void AXPGen::gen(PNode p) 
{
	switch (*p) {
		case Nconst: genConst(PConstExpr(p));	break;
		case Nsymbol: genSymbol(PSymExpr(p));	break;
		case Nassign: genAssign(PBinExpr(p)); break;
		case Nblock: genBlock(PBlock(p)); break;
		case Nexpr: genExpr(PExprStat(p)); break;
		case Ndeclstat: break;
		case Naddr: genDeref(PUnExpr(p)); break;
		case Ncall:		genCall(PBinExpr(p)); break;
		case Ncast: genCast(PUnExpr(p)); break;
		case Nreturn: genReturn(PExprStat(p)); break;
		case Nneg: genNegate(PUnExpr(p));	break;
		case Nbinnot: genBinNot(PUnExpr(p));	break;
		case Nadd:
		case Nsub:	
		case Nmul: 
		case Ndiv: 
		case Nmod: 
		case Nbinand:
		case Nbinor:
		case Nbinxor:  
		case Nshl:
		case Nshr:	genBinOp(PBinExpr(p)); break;
		case Ncomma:    genComma(PComma(p)); break;
		case Ncmp: genCompare(p); break;
		case Norelse1: genOrElse1(p); break;
		case Norelse2: genOrElse2(p); break;		
		case Nif:
		case Nwhile:
		case Nfor:
		case Ndo:	genCondIstr(p); break;
		case Nexpr2bool:	genToBool(p); break;
		// case Ninitvar:  genInitVar(PInitStatement(p)); break;
		case Nbool2expr: genFromBool(p); break;
		default:
			break;
	}	
}

// emette la costante
void AXPGen::genConst(PConstExpr p)
{	
	if(asmTypeOf(p->ptipo) == AFLOAT) {
		obuf << "ldis\t" << "$f0,";
		emitConst(p);
		newline();
	}
	else { 
		obuf << "bis\t$31,";
		emitConst(p);
		obuf << ',' <<(asmTypeOf(p->ptipo) == AFLOAT ? "$f0": "$0");		
		newline();
	}
	emitPush(0, asmTypeOf(p->ptipo));

}

void AXPGen::genSymbol(PSymExpr p)
{	
	AsmType t = asmTypeOf((*p)->getTipo());
	if (p->lvalue()) {
		emitIstr("lda", 0, p->getSymbol());	newline();		
	}
	else {		
		if(t == ACHAR) {
			emitIstr("lda", rT1, p->getSymbol()); newline();
			emitCharLoad(rT1, rT2, 0);		
		}
		else {
			emitIstr(makeTypedIstr("ld", t), 0, p->getSymbol(), t == AFLOAT);		
			newline();
		}				
	}
	emitPush(0, p->lvalue() ? APTR : t);	

}

void AXPGen::genNegate(PUnExpr p)
{
	/*
	gen(p->sx());
	switch(asmTypeOf(p->ptipo)) {
	case ACHAR:		
		obuf << "neg\tbyte ptr [esp]"; newline();
		break;
	case AINT:
		obuf << "neg\tdword ptr [esp]"; newline();
		break;
	case AFLOAT:
		emitLoadFpu();
		obuf << "fchs"; newline();
		emitStoreFpu();
		break;
	default:
		// TODO: error
		break;
	}
	*/
}

void AXPGen::genBinNot(PUnExpr p)
{
	gen(p->sx());
	switch(asmTypeOf(p->ptipo)) {
	case ACHAR:
	case AINT:
		emitPop(0);
		emitIstr("not", 0,0, false); newline();
		break;
	default:
		// TODO: error
		break;
	}
}


void AXPGen::genComma(PComma p)
{
	CComma::iterator it = p->begin();
	while(it != p->end()) {
		gen(*it);
		it++;
	}
	
}

void AXPGen::genBinOp(PBinExpr p)
{
	gen(p->sx());
	gen(p->dx());
	
	// costruzione del mnemonico
	const char *cp;	
	AsmType t;
	switch(*p) {
	case Nadd: cp =  "add"; break;
	case Nsub: cp = "sub"; break;
	case Nmul: cp = "mul"; break;
	case Ndiv: cp = "div"; break;
	case Nmod: cp = "rem"; break;
	case Nbinor: cp = "bis";break;
	case Nbinand: cp = "and"; break;
	case Nbinxor: cp = "xor"; break;
	case Nshl: cp = "sll"; break;
	case Nshr: cp = "srl"; break;
	}

	if(t != AFLOAT) {
		emitPop(rT0,t);
		emitPop(rT1,t);
		emitIstr( makeTypedIstr(cp, t), rT0, rT1, rT0);
		newline();
		emitPush(rT0, t);
	}
	else {
		emitPop(0, AFLOAT);
		emitPop(1, AFLOAT);
		emitIstr(makeTypedIstr(cp, t), 0,1,0);
		newline();
		emitPush(0, AFLOAT);		
	}
}


// typecasting 
// calcoliamo il tipo assembler delle due espressioni
// se la destinazione e' un 
void AXPGen::genCast(PUnExpr p)
{
	// TODO
}

void AXPGen::genReturn(PExprStat p)
{
	gen(p->expr);

	// in base al tipo ritorna
	AsmType t = asmTypeOf(p->expr->ptipo);
	if(t != AVOID) 
		emitPop(0,t);	
}


void AXPGen::genCall(PBinExpr p)
{	
	PType proto;
	int size = 0;
	PComma pc = (PComma)p->dx();

	if(p->sx()->ptipo->isPtr())
		proto = p->sx()->ptipo->deref();
	else
		proto = p->sx()->ptipo;

	CComma::iterator it = pc->begin();

	// nel passaggio ad una funzione il mio 
	for(;it != pc->end();it++) {
		gen(*it);		
		size += 8;
	}


	int onreg = min(6, proto->getParamCount())-1;
	int base = (proto->getParamCount()-onreg-1)*8;
	
	for( int i = onreg; i >= 0; i--) {
		AsmType t = asmTypeOf(proto->getParameter(i)); 
		emitIstr(makeTypedIstr("ld", t),
			(t == AFLOAT ? 16: rA0)+i, memory(rS0, base));
		newline();
		base += 8;
	}
	

	
		
	emitIstr("bis", rS0, rS0, 0); newline();
	emitIstr("bis",rSP, rSP, rS0);newline();
	emitIstr("bis", 0,0, rSP);newline();
	if(*p->sx() == Nsymbol) {
		emitIstr("jsr", 26, asSymbol(p->sx()));	
		newline();
	}
	else {
		gen(p->sx());
		emitPop(rT0, APTR);
		emitIstr("jsr", 26, memory(rT0,0)); newline();
	}
	emitIstr("bis", rS0, rS0, rT0);newline();
	emitIstr("bis",rSP, rSP, rS0);newline();
	emitIstr("bis", rT0, rT0, rSP);newline();
	emitIstr("addq", rS0, imm(8*proto->getParamCount()), rS0);newline();
	// ripristino GOT (in ogni caso, non distinguo fra extern 
	// e local functions)
	emitIstr("ldgp", rGP, memory(rRA,0)); newline();	
	emitIstr("bis", rFP, rFP, rSP);newline();
	emitPush(0, asmTypeOf(proto->functionReturn()));

}


//*****************************************************************

char *AXPGen::makeTypedIstr(const char *src, AsmType t)
{
static char buf[20];
	std::ostrstream os(buf, sizeof (buf));
	os << src;
	os << (t == AFLOAT ? 's': (t == APTR ? 'q' : 'l')) << std::ends;
	return buf;
}

void AXPGen::emitCharLoad(Registro addr, Registro tmp, Registro dst)
{
	emitIstr("ldq_u", tmp, memory(addr,0));
	emitIstr("extqh", tmp, addr, tmp);
	emitIstr("sra", tmp, imm(56), dst);
}

void AXPGen::emitCharStore(Registro addr, Registro tmp, Registro src)
{
	emitIstr("ldq_u", tmp, memory(addr, 0));
	emitIstr("maskbl", tmp,  addr, tmp);
	emitIstr("insbl", src, addr, src);
	emitIstr("bis", src, tmp, src);
	emitIstr("stq_u", src, memory(addr,0));
}
	
void AXPGen::emitStore(Registro addr, Registro tmp, Registro src, AsmType t)
{
	if(t == ACHAR) 
		emitCharStore(addr, tmp, src);
	else {
		emitIstr(makeTypedIstr("st", t), 
			src, memory(addr,0), t == AFLOAT);
		newline();
	}	
}

void AXPGen::emitPop(Registro reg, AsmType t )
{
	

	emitIstr(makeTypedIstr("ld",t), reg, memory(rS0, 0), t == AFLOAT); 
	newline();
     emitIstr("addq", rS0, imm(8), rS0);		  newline();
}

void AXPGen::emitTop(Registro reg, AsmType t)
{
	emitIstr(makeTypedIstr("ld", t), reg, memory(rS0, 0)); 
	newline();
	
}

void AXPGen::emitPush(Registro reg, AsmType t)
{
	emitIstr("subq", rS0, imm(8), rS0);		  newline();
	emitIstr(makeTypedIstr("st",t), reg, memory(rS0, 0),t == AFLOAT); newline();
}



// emitconst: genera il testo della costante
void AXPGen::emitConst(PConstExpr p)
{
	char c;
	switch(p->ptipo->getType()) {
	case tPTR:
	case tINT: obuf  << std::dec << p->ival;
			   ocomment << "// " << p->ival;
			   break;
	case tCHAR: obuf  << std::dec << int(p->cval);
				ocomment << "// \'";
				c = p->cval;
				if(c< 20 || c> 127)
					ocomment << "x" << std::hex << int(c);
				else 
					ocomment << c;
				ocomment << '\'';
				break;
	case tFLOAT:
		/*
			k = *((int *)&p->fval);		// 32bit <-> 32bit
			obuf << '0' << hex << k << "H";
			ocomment << "; " << p->fval;
		*/
			break;
	}
}

// emitvar: genera il testo corrispondende al simbolo specificato
//	 se c'e il flag lvalue ed e' statica o globale o extern premette
//		"offset"
//	 altrimenti "size ptr" dove size puo' essere byte o dword	
// La versione overloaded controlla che il nodo sia di tipo Nsymbol e
// richiama emitvar passando rvalue secondo l'informazione nel nodo
void AXPGen::emitVar(PSymbol sym, bool rvalue)
{
	assert(sym);
	if(rvalue || (!rvalue && sym->isLocal())) {
		obuf << (sym->getTipo()->getType() == tCHAR ? "BYTE" : "DWORD")
			<< " PTR ";
		obuf << sym->asmName;
		// Se e' PARAM o LOCALE non statica
		if(sym->isLocal()) 
			obuf << "[ebp]";		
	}
	else {
		obuf << "OFFSET " << sym->asmName;
	}
}

void AXPGen::emitVar(PSymExpr p)
{
	emitVar(p->getSymbol(), p->rvalue());
}

const char * AXPGen::registerString(Registro reg, bool isFloat)
{
static const char * name[32] = 
	{
		"$$0", "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", 
		"$t6", "$t7", "$s0", "$s1", "$s2", "$s3", "$s4",
		"$s5", "$fp", "$a0", "$a1", "$a2", "$a3", "$a4",
		"$a5", "$t8", "$t9", "$t10", "$t11", "$ra",
		"$pv", "$AT", "$gp", "$sp", "$31"
	};
static char buf[5];

	if(isFloat) {
		std::ostrstream os(buf, sizeof(buf));
		os << "$f" << std::dec << int(reg) << std::ends;
		return buf;
	}
	else
		return name[reg]+1;
}

void AXPGen::genAssign(PBinExpr p)
{
	gen(p->dx());
	gen(p->sx());
	
	AsmType t = asmTypeOf(p->ptipo);
	emitPop(rT0, APTR);
	emitTop(rT1, t);
	emitStore(rT0, rT2, rT1, t);
	newline();
}

void AXPGen::genBlock(PBlock p)
{
	CBlock::iterator it = p->begin();
	while(it != p->end()) {
		gen(*it);
        ++it;
	}
}

void AXPGen::genExpr(PExprStat p)
{
	gen(p->expr);

	// stack recover if something on stack
	if(p->expr->ptipo != CType::Void()) {
		emitPop(0);
	}
}


void AXPGen::emitIstr(const char * text, Registro r,PSymbol s, bool isFloat)
{
	if(s->isLocal()) 
		emitIstr(text, r, memory(rFP, s->offset), isFloat);
	else {
		obuf << text << '\t' << registerString(r,isFloat) << ',';
		obuf << mangleSymbol(s);		
		addComment(s->getNome());
	}
	addComment(s->getNome());
}

void AXPGen::emitIstr(const char * text, Registro r,memory mem, bool isFloat)
{
	obuf << text << '\t' << registerString(r,isFloat) 
		<< ',' << std::dec << int(mem.offset) << '(' << 
		registerString(mem.index) << ')';
}

void AXPGen::emitIstr(const char * text, Registro r_src, Registro r_src2, Registro r_dst, bool isFloat)
{
	obuf << text << '\t' << registerString(r_src,isFloat); 
	obuf << ',' << registerString(r_src2,isFloat) << ',' << 
		registerString(r_dst,isFloat);
}

void AXPGen::emitIstr(const char * text, Registro r_src, imm immed, Registro r_dst)
{
	obuf << text << '\t' << registerString(r_src); 
	obuf << ", " << std::dec << int(immed) << ',' << 
		registerString(r_dst);
}

void AXPGen::emitIstr(const char * text, Registro r_src, Registro r_dst, bool isFloat)
{
	obuf << text << '\t' << registerString(r_dst, isFloat); 
	obuf << ',' << registerString(r_src, isFloat);		
}

void AXPGen::emitIstr(const char * text, imm immed, Registro r_dst)
{
	obuf << text << '\t' << registerString(r_dst); 
	obuf << ", " << std::dec << int(immed);		
}

	




void AXPGen::genDeref(PUnExpr p)
{
	gen(p->sx());
	emitPop(rT2, APTR);		
	emitStore(rT2, rT1, rT0, asmTypeOf(p->ptipo)); newline();
	emitPush(rT0, asmTypeOf(p->ptipo));
}

void AXPGen::genCompare(PRelBoolExpr p)
{
	// TODO
}

// l'emissione di una label vuol dire che in questa posizione
// ognuna delle TempLabel in lset devono essere associate ad
// una data label fisica. 
// In questa implementazione LabelTemp
void AXPGen::emitLabel(LabelSet lset)
{
	unsigned j;
	lset.begin(j);
	while(lset.ok(j)) {
		int idx = lset.next(j);
		emitLabel(idx);
	}
}

void AXPGen::emitLabel(const TrueLabel lbl)
{
	obuf << std::dec << lbl << ":"; newline();
}

TrueLabel AXPGen::extractFirstLabel(LabelSet lset)
{
	unsigned j;
	lset.begin(j);
	if(lset.ok(j)) 
		return lset.next(j);			
	else
		return 0;
}

void AXPGen::emitJCond(const  LogicDesc & ld)
{
	emitJCond(ld.condizione, ld.vero, ld.floatTest);
}

void AXPGen::emitJCond(Cond c, LabelSet lset, bool isFloat )
{
	// TODO
}

void AXPGen::genOrElse1(PBoolExpr p)
{
	gen(p->sx());
	if(p->ldesc != cFALSE) 
		emitJCond(p->ldesc);
	emitLabel(p->ldesc.falso);	
}

void AXPGen::genOrElse2(PBoolExpr p)
{
	gen(p->sx());
	gen(p->dx());
}

void AXPGen::genToBool(PExpr2Bool p)
{
	// TODO
}

// expr
// * salta a falso 
// LBL: vero 
//   setta a vero
//   jmp $next
// LBL: falso
//   setta a falso
// $next
void AXPGen::genFromBool(PBool2Expr p)
{
	// TODO
}

// in base al tipo di nodo genera il codice
void AXPGen::genCondIstr(PCondStat p)
{
	LogicDesc desc = p->bcond->ldesc;
	switch(*p) {
	case Nif:
		gen(p->bcond);
		emitJCond(!desc.condizione, desc.falso, desc.floatTest);
		emitLabel(desc.vero);
		gen(p->block);
		if(p->blockII) {
			emitJCond(cTRUE, desc.unisci);
			emitLabel(desc.falso);
			gen(p->blockII);
		}
		else
			emitLabel(desc.falso);
		emitLabel(desc.unisci);
		break;
	case Nwhile:
		emitLabel(desc.unisci);
		gen(p->bcond);
		emitJCond(!desc.condizione, desc.falso, desc.floatTest);
		emitLabel(desc.vero);
		gen(p->block);
		emitJCond(cTRUE, desc.unisci);
		emitLabel(desc.falso);
		break;
	case Ndo:
		emitLabel(desc.vero);
		gen(p->block);
		gen(p->bcond);
		emitJCond(desc);
		emitLabel(desc.falso); 
		break;

	}
}
