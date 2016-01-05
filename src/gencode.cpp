// Nome: gencode.cpp
// Progetto: CPP/Generator
// Autore: Ruffaldi Emanuele
// Generatore di codice per Intel x86
#include "gencode.h"
#include <string.h>


static char * prefix = // prima di tutto TITLE nomefile sourcenge
".386P\n"
"if @Version gt 510\n"
".model FLAT\n"
"else\n"
"_TEXT	SEGMENT PARA USE32 PUBLIC \'CODE\'\n"
"_TEXT	ENDS\n"
"_DATA	SEGMENT DWORD USE32 PUBLIC \'DATA\'\n"
"_DATA	ENDS\n"
"CONST	SEGMENT DWORD USE32 PUBLIC \'CONST\'\n"
"CONST	ENDS\n"
"_BSS	SEGMENT DWORD USE32 PUBLIC \'BSS\'\n"
"_BSS	ENDS\n"
"_TLS	SEGMENT DWORD USE32 PUBLIC \'TLS\'\n"
"_TLS	ENDS\n"
"FLAT	GROUP _DATA, CONST, _BSS\n"
"	ASSUME	CS: FLAT, DS: FLAT, SS: FLAT\n"
"endif\n";

static char * postfix = "END";


CodeGen::CodeGen(char * source, char *output, PBlock pb) :
	out(output) , obuf(outbuf, sizeof(outbuf)), ocomment(comment, sizeof(comment)),
		inCode(false), ftoldeclared(false), bool2exprlabelcount(0)
, stringStorage()
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
	// inserisci [ftol] ofunctionCode
	if(ftoldeclared) 
		out << "extrn __ftol";	
	ofunctionCode << ends;
	out << ofunctionCode.str();
	postCode();
}

bool CodeGen::ready() {return !out.fail();}

// Function name	: CodeGen::preCode
// Description	    : output the fixed part of the file
// Return type		: void 
void CodeGen::preCode()
{
	out << prefix;
}

void CodeGen::postCode()
{
	// Ora le stringhe
	out << "_DATA\tSEGMENT\n";
	for(int i = 0 ; i < stringStorage.size(); i++) {
		CConstExpr * pe  = stringStorage[i];
		out << "$SG" << dec << i << "\tDB\t";
		const char * cp = pe->strval.cp;
		const char * endcp = cp + pe->strval.len;
		bool inString = false;
		char first = ' ';
		while(cp != endcp) {
			char c = *cp++;
			// printable
			if(c > 20 && c != '\'') {
				if(!inString) {
					out << first << " \'";
					inString = true;
				}
				out << c;
			}
			// not
			else {
				if(inString) {
					out << "\'";
					inString = false;
				}
				out << first;
				char buf[4] = "XXH";
				char tc = c & 0xF;
				buf[1] = tc >= 0xA ? 'A' + (10 - tc): '0' + tc;
				tc = (c >> 4) & 0xF;
				buf[0] = tc >= 0xA ? 'A' + (10 - tc) : '0' + tc;
				if(tc >= 0xA) out << '0';	
				out << " " << buf;				
			}
			first = ',';
		}
		// append 0
		out << endl;
	}
	out << "_DATA\tENDS\n";
	out << postfix;
}
	
char * CodeGen::mangleSymbol(CSymbol *ps)
{
	return ps->asmName;
}

void CodeGen::addComment(char * s)
{
	ocomment << ";" << s;	
}

void CodeGen::newline()
{
	int n = obuf.pcount();
	if(n < 40) {
		for(int i = 0; i < 40-n; i++)
			obuf << ' ';
	}
	ocomment << ends;
	obuf << ocomment.str() << ends;	
	ofunctionCode << (inCode ? "\t\t" : "") << obuf.str() << endl;
	obuf.seekp(0);
	ocomment.seekp(0);
}

// Function name	: CodeGen::printPublicVars
// Description	    : stampa le dichiarazioni public per le funzioni
//						globali non statiche
// Return type		: void 
void CodeGen::printPublicFun()
{
	slista<CFunction *>::iterator it = funDefinite.begin();
	while(it != funDefinite.end()) {
		CFunction *psym = *it;
		if(!psym->isStatic() && !psym->proto) {
			obuf << "PUBLIC " << mangleSymbol(psym);
			addComment(psym->getNome());
			newline();
		}
        ++it;
	}
}

// Function name	: CodeGen::printPublicVars
// Description	    : stampa le dichiarazioni extern
// Return type		: void 
void CodeGen::printExternSym()
{
	slista<CSymbol*>::iterator it = symEsterni.begin();
	while(it != symEsterni.end()) {
		CSymbol *psym = *it;		
		obuf << "EXTRN " << mangleSymbol(psym) << ":";
		if(psym->isFunction()) 
			obuf << "NEAR";
		else {
			AsmType tp = asmTypeOf(psym->getTipo());
			obuf << (tp == ACHAR ?  "BYTE": "DWORD");			
		}
		addComment(psym->getNome());
		
		newline();		
        ++it;
	}
}


// Function name	: CodeGen::printPublicVars
// Description	    : stampa le dichiarazioni public per le variabili
//						globali non statiche
// Return type		: void 
void CodeGen::printPublicVar()
{
	slista<CSymbol *>::iterator it = globalVar.begin();
	while(it != globalVar.end()) {
		CSymbol *psym = *it;
		obuf << "PUBLIC " << mangleSymbol(psym);
		addComment(psym->getNome());
		newline();
        ++it;
	}
}

void CodeGen::definePublicVar()
{
	slista<CSymbol *>::iterator it = globalVar.begin();
	obuf << "_DATA SEGMENT\n";
	while(it != globalVar.end()) {
		CSymbol *psym = *it;
		AsmType tp = asmTypeOf(psym->getTipo());
		int elemCount = 1;
		PType t = psym->getTipo();

		while(t->isArray()) {
			elemCount *= t->getArraySize();
			t = t->deref();
		}

		obuf << mangleSymbol(psym);
		obuf << '\t' << (tp == ACHAR ? "DB" : "DD") << "\t";
		if(psym->codice) 
			cout << "Init values not implemented\n";		
		if(elemCount == 1) 
			obuf << "0";
		else
			obuf << '0' << hex << elemCount << dec << "h DUP (0)";
		addComment(psym->getNome());
		newline();
        ++it;
	}
	obuf << "_DATA ENDS\n";
}

// where = 0 root, 1 funzione, 2 blocco
// * Calcola asmName per ogni simbolo
// * Classifica: static / extern / globalVar
void CodeGen::processSymbols(PBlock pb, int where)
{
    
    FOR_EACH(psym, pb->getSymTab(), CSymbolTable::iterator, CSymbol*)
 		ostrstream ost;
		if(psym->isFunction()) {
			if(psym->isExtern()) {
				if(psym->used)
					symEsterni.push_back(psym);
			}
			else 
				funDefinite.push_back((CFunction*)psym);						
			ost << "_" << psym->getNome() << ends;
			psym->asmName = ost.str();
		}
		// variabili globali
		else if(where == 0) {
			if(psym->sType() == stoNORMAL)
				globalVar.push_back(psym);
			else if(psym->sType() == stoEXTERN)
				symEsterni.push_back(psym);		
			ost << "_" << psym->getNome() << ends;	
			psym->asmName = ost.str();
		}	
		// simboli locali ad una funzione
		else {			
			if(psym->sType() == stoEXTERN) 
				symEsterni.push_back(psym);			
			// calcola mangled name come in processLocalBlock
			ostrstream ost;
			ost << "_" << psym->getNome() << "$";
			if(where == 2)
				ost << dec << pb->id();
			ost << ends;
			psym->asmName = ost.str();

		}        
	END_EACH();

	// Analizza tutti i sotto blocchi
	CBlock::blkIterator it2 = pb->blkBegin();
    while(it2 != pb->blkEnd()) {
        processSymbols(PBlock(*it2), 
			where == 0 ? 1 : 2);
        ++it2;
	}
}


// Function name	: CodeGen::asmtypeof
// Description	    : ritorna il tipo assembler per un item x
// Return type		: AsmType 
// Argument         : PType
CodeGen::AsmType CodeGen::asmTypeOf(PType t)
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
		cout << "INTERNAL ERROR IN asmtypeof\n";
		return AVOID;
	}
}

void CodeGen::processFunctions()
{
	slista<CFunction *>::iterator it = funDefinite.begin();
	ofunctionCode << "_TEXT SEGMENT\n";
	while(it != funDefinite.end()) {
		CFunction *psym = *it;
		if(psym->inited) {
			// processa il blocco della funzione
			prolog(psym);			
			gen(PNode(psym->getBody()));			
			epilog(psym);			
			newline();
		}
        ++it;
	}
	ofunctionCode << "_TEXT ENDS\n";
}

void CodeGen::prolog(CFunction *psym)
{
	processLocalSymbols(psym);
	obuf << mangleSymbol(psym) << " PROC NEAR";
	addComment(psym->getNome());
	newline();
	inCode = true;
	obuf << "push\tebp"; newline();
	obuf << "mov\tebp, esp";newline();
	if(psym->frameSize != 0) {
		obuf << "sub\tesp, " << dec << psym->frameSize;		
		newline();
	}
}

void CodeGen::epilog(CFunction *psym)
{
	// ripulisci lo stack, con la dimensione del callframeSize
	obuf << "$uscita:"; newline();
	if(psym->frameSize != 0) { 
		obuf << "mov\tesp, ebp"; newline();
	}
	obuf<< "pop\tebp"; newline();

	if(psym->getTipo()->getCallMode() == csCdecl) 
		obuf << "ret";	
	else {
		int callFrame = psym->argSize;		
		obuf << "ret\t" << dec << callFrame;
	}
	newline();
	inCode = false;
	obuf << mangleSymbol(psym) << " ENDP";
	addComment(psym->getNome());
	newline();
}

int CodeGen::fullSizeOf(PType t)
{
	int k = 1;
	while(t->isArray()) {
		k *= t->getArraySize();
		t = t->deref();
	}
	switch(k) {
	case tINT: case tFLOAT: case tPTR: case tFUNC: return k*4;
	case tCHAR: return k;
	default: return 0;
	}

}

int CodeGen::argSizeOf(PType)
{
	return 4;
}

void CodeGen::emitSymOffset(CSymbol* psym)
{
	ofunctionCode << mangleSymbol(psym);
	ofunctionCode << " = " << dec << (psym->offset) << "\n";
}



// Function name	: CodeGen::processLocalSymbols
// Description	    : calcola offset delle variabili locali
//					  e calcola/stampa i nomi dei simboli locali
// Return type		: void 
// Argument         : CFunction *pfunc
// 
// Un argomento o variabile locale e' mangled. 
// _ nome $ codiceblocco	=	offset sullo stack
void CodeGen::processLocalSymbols(CFunction *pfunc)
{
	processLocalBlock(PBlock(pfunc->codice), -4, true);
	// ora una scansione solo per gli argomenti
	bool isCDecl = pfunc->getTipo()->getCallMode() == csCdecl;
	int varOff = !isCDecl ? pfunc->argSize: 8;
    CSymbolTable::iterator it = pfunc->getTable().begin();
    CSymbolTable::iterator ite = pfunc->getTable().end();

    while(it != ite) {
        CSymbol * psym = *it;
		if(psym->isParam()) {
			if(!isCDecl) {
				int n = argSizeOf(psym->getTipo());
				psym->offset = varOff-n;
				varOff -= n;
			}
			else {
				psym->offset = varOff;
				varOff += argSizeOf(psym->getTipo());
			}
			emitSymOffset(psym);
		}
        ++it;
	}
	
}


// Function name	: CodeGen::processLocalBlock
// Description	    : 
// Return type		: void 
// Argument         : PBlock pb
// Argument         : int startoff
// Argument         : bool topmost = false
void CodeGen::processLocalBlock(PBlock pb, int startOff, bool topmost)
{
	FOR_EACH(psym, pb->getSymTab(), CSymbolTable::iterator, CSymbol*)        
		if(psym->sType() == stoNORMAL) {
			// Calcola offset
			psym->offset = startOff;
			startOff -= fullSizeOf(psym->getTipo());

			// Produci il simbolo macro
			emitSymOffset(psym);
		}
    END_EACH();

	// Analizza tutti i sotto blocchi
	CBlock::blkIterator itb = pb->blkBegin();
    while(itb != pb->blkEnd()) {
        processLocalBlock(PBlock(*itb), startOff, false);
        ++itb;
	}
}


// CODE GENERATIONS: by node
void CodeGen::gen(PNode p) 
{
	switch (*p) {
		case Nconst: genConst(PConstExpr(p));	break;
		case Nsymbol: genSymbol(PSymExpr(p));	break;
		case Nassign: genAssign(PBinExpr(p)); break;
		case Nblock: genBlock(PBlock(p)); break;
		case Nexpr: genExpr(PExprStat(p)); break;
		case Ndeclstat: break;
		case Nadd:
		case Nsub:	genAddSub(PBinExpr(p));		break;
		case Naddr: genDeref(PUnExpr(p)); break;
		case Ncall:		genCall(PBinExpr(p)); break;
		case Ncast: genCast(PUnExpr(p)); break;
		case Nreturn: genReturn(PExprStat(p)); break;
		case Nneg: genNegate(PUnExpr(p));	break;
		case Ninc:
		case Ndec:
				genPreIncDec(PUnExpr(p)); break;
		case Nbinnot: genBinNot(PUnExpr(p));	break;
		case Nmul: genMul(PBinExpr(p)); break;
		case Ndiv: 
		case Nmod: genDivMod(PBinExpr(p)); break;
		case Nbinand:
		case Nbinor:
		case Nbinxor:  genBinary(PBinExpr(p));	break;
		case Nshl:
		case Nshr:	genShift(PBinExpr(p)); break;
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
void CodeGen::genConst(PConstExpr p)
{
	obuf << "push\t";
	emitConst(p);
	newline();
}

void CodeGen::genSymbol(PSymExpr p)
{	
	if (p->lvalue()) {		
		if((*p)->isLocal()) {
			obuf << ("lea\teax, ");
			emitVar(p->getSymbol());
			newline();
			emitPush(EAX);
		}
		else {
			obuf << "push\t";
			emitVar(p);			
		}
		newline();
	}
	else 
		emitPush(p->getSymbol());		

}

void CodeGen::genNegate(PUnExpr p)
{
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
}

void CodeGen::genBinNot(PUnExpr p)
{
	gen(p->sx());
	switch(asmTypeOf(p->ptipo)) {
	case ACHAR:
	case AINT:
		obuf << "not\tdword ptr [esp]"; newline();
		break;
	default:
		// TODO: error
		break;
	}
}


void CodeGen::genComma(PComma p)
{
	CComma::iterator it = p->begin();
	while(it != p->end()) {
		gen(*it);
		it++;
	}
	
}

void CodeGen::genBinary(PBinExpr p)
{
	assert(asmTypeOf(p->ptipo) != AFLOAT);
	gen(p->sx());
	gen(p->dx());
	char * op;
	switch(*p) {
	case Nbinor: op = "or";break;
	case Nbinand: op = "and"; break;
	case Nbinxor: op = "xor"; break;
	default:
		// TODO: errore
		break;
	}
	emitPop(EAX);
	obuf << op << "\tdword ptr [esp], eax"; newline();		
}

void CodeGen::genMul(PBinExpr p)
{
	assert(asmTypeOf(p->sx()->ptipo) == asmTypeOf(p->dx()->ptipo));
	gen(p->sx());
	gen(p->dx());
	switch(asmTypeOf(p->ptipo)) {
	case AINT:
	case ACHAR:
		emitPop(EAX);
		obuf << "imul\teax, dword ptr [esp]"; newline();
		obuf << "mov\tdword ptr [esp], eax"; newline();
		break;
	case AFLOAT:
		emitLoadFpu();
		emitPop(EAX);
		obuf << "fmul\tdword ptr [esp]"; newline();
		emitStoreFpu();
		break;
	default:
		// TODO: error
		;
	}
}

void CodeGen::genDivMod(PBinExpr p)
{
	assert(asmTypeOf(p->sx()->ptipo) == asmTypeOf(p->dx()->ptipo));
	gen(p->sx());
	gen(p->dx());
	if (*p == Ndiv) {
		switch(asmTypeOf(p->ptipo)) {
		case ACHAR:
			emitPop(EAX);
			emitPop(EBX);		
			obuf <<"idiv\tbl"; newline();
			emitPush(EAX);
			break;
		case AINT:
			emitPop(EAX);
			emitPop(EBX);
			obuf << "cdq"; newline();
			obuf << "idiv\tebx"; newline();
			emitPush(EAX);
			break;
		case AFLOAT:
			emitLoadFpu();
			emitPop(EAX);
			obuf << "fdivp\tdword ptr [esp]"; newline();
			emitStoreFpu();
			break;
		default:
			// TODO: error
			break;
		}
	}
	else {
		switch(asmTypeOf(p->ptipo)) {
		case ACHAR:
			emitPop(EAX);
			emitPop(EBX);		
			obuf << "idiv\tbl"; newline();
			obuf << "xchg\tal, ah"; newline();
			emitPush(EAX);
			break;
		case AINT:
			emitPop(EAX);
			emitPop(EBX);
			obuf <<"cdq"; newline();
			obuf << "idiv\tebx"; newline();
			emitPush(EDX);
			break;
		default:
			// TODO: error
			break;
		}
	}
}

void CodeGen::genShift(PBinExpr p)
{
	assert(asmTypeOf(p->ptipo) != AFLOAT);
	assert(asmTypeOf(p->sx()->ptipo) == asmTypeOf(p->dx()->ptipo));
	gen(p->sx());
	gen(p->dx());
	char * op = *p == Nshl ? "shl" : "sar";
	emitPop(ECX);
	obuf << op << "\tdword ptr [esp], cl";
	newline();
}

// typecasting 
// calcoliamo il tipo assembler delle due espressioni
// se la destinazione e' un 
void CodeGen::genCast(PUnExpr p)
{
		AsmType src,dst;	
	gen(p->sx());
	dst = asmTypeOf(p->ptipo);
	src = asmTypeOf(p->sx()->ptipo);
	if(src == ACHAR) {
		switch(dst) {
		case AINT:
		case APTR:
			emitPop(EAX);
			obuf << "movsx\teax, al"; newline();
			emitPush(EAX);
			break;
		case AFLOAT:
			emitPop(EAX);
			obuf << "movsx\teax, al"; newline();
			emitPush(EAX);
			obuf << "fild\tdword ptr [esp]"; newline();
			emitStoreFpu();
			break;
		case AVOID:
			emitPop(EAX);
			break;
		default:
			// TODO: error			
			cout << "Error in generator\n";
			break;
		}
	}
	else if(src == AINT) {
		switch(dst) {
		case ACHAR: 
		case APTR: break;
		case AFLOAT:
			obuf << "fild\tdword ptr [esp]"; newline();
			emitStoreFpu();
			break;
		case AVOID:
			emitPop(EAX); break;
		default:
			// TODO: errore
			cout << "Error in generator\n";
			break;
		}
	}
	else if(src == AFLOAT) {
		switch(dst) {
		case AINT: 
			// TODO: dichiara la funzione __ftol
			declareFTOL();
			emitLoadFpu();
			obuf << "call\t__ftol";	
			newline();
			emitStoreFpu();
			break;
		case AVOID:
			emitPop(EAX); break;
		default:
			// TODO: errore
			cout << "Error in generator\n";			
			break;
		}
	}
	else if(src == APTR && dst == AINT)
		;	// donothing
	else
		cout << "Error in generator\n";			

}

void CodeGen::genReturn(PExprStat p)
{
	gen(p->expr);
	// in base al tipo ritorna
	AsmType tp = asmTypeOf(p->expr->ptipo);
	switch(tp) {
	case AVOID: break;
	case ACHAR: 
	case AINT:
	case APTR:
		emitPop(EAX);
		break;
	case AFLOAT:
		emitLoadFpu();
		break;
	}
	obuf << "jmp\t$uscita"; newline();
}

void CodeGen::genCall(PBinExpr p)
{	
	PType proto;
	bool bcdecl;
	int size = 0;
	PComma pc = (PComma)p->dx();

	if(p->sx()->ptipo->isPtr())
		proto = p->sx()->ptipo->deref();
	else
		proto = p->sx()->ptipo;

	bcdecl = proto->getCallMode() == csCdecl;
	if(bcdecl) {
		// inverto gli argomenti per la cdecl
		if(pc) pc->inverti();
	}	
	if(pc) {
	CComma::iterator it = pc->begin();
	for(int i = 0;it != pc->end();it++,i++) {
		gen(*it);
		// Semplicemente vanno lasciati sullo stack!
		// size += argSizeOf(it->proto->getParameter(i));
		size += argSizeOf((*it)->ptipo);
	}
	}
	if(p->sx()->node() == Nsymbol) {
		obuf << "call\t"; 
		obuf << mangleSymbol(PSymExpr(p->sx())->getSymbol());		
		newline();
	}
	else {
		gen(p->sx());
		emitPop(EAX);
		obuf << "call\tdword ptr [eax]"; newline();
	}

	// ripulisce lo stack solo se cdecl
	if(bcdecl) 
		obuf << "add\tesp, " << dec << size; newline();	
	AsmType t = asmTypeOf(proto->functionReturn());
	if(t == AFLOAT) {
		emitPush(EAX);
		emitStoreFpu();
	}
	else if(t != AVOID) 
		emitPush(EAX);
	
}

// EMITTERS 

// push del registro...+ newline
void CodeGen::emitPush(Registro reg)
{
	obuf << "push\t" << registerString(reg, 4);	
	newline();
}

// la emitPush di un simbolo genera:
//	push + emitVar(symbol)
// sempre come rvalue
// +newline
void CodeGen::emitPush(PSymbol p)
{
	obuf << "push\t";
	emitVar(p);
	newline();
}

// emitconst: genera il testo della costante
void CodeGen::emitConst(PConstExpr p)
{
	char c;
	unsigned k;
	switch(p->ptipo->getType()) {
	case tINT: obuf << dec << p->ival;
			   ocomment << "; " << p->ival;
			   break;
	case tCHAR: obuf << dec << int(p->cval);
				ocomment << "; \'";
				c = p->cval;
				if(c< 20 || c> 127)
					ocomment << "0" << hex << int(c) << "H" ;
				else 
					ocomment << c;
				ocomment << '\'';
				break;
	case tFLOAT:
			k = *((int *)&p->fval);		// 32bit <-> 32bit
			obuf << '0' << hex << k << "H";
			ocomment << "; " << p->fval;
			break;
	case tPTR:
		// stringa, inseriamola nel db
		{
			int code = stringStorage.size();
			stringStorage.pushback((CConstExpr*)p);
			obuf << "OFFSET FLAT:$SG" << dec << code;
		}
		break;
	}
}

// emitvar: genera il testo corrispondende al simbolo specificato
//	 se c'e il flag lvalue ed e' statica o globale o extern premette
//		"offset"
//	 altrimenti "size ptr" dove size puo' essere byte o dword	
// La versione overloaded controlla che il nodo sia di tipo Nsymbol e
// richiama emitvar passando rvalue secondo l'informazione nel nodo
void CodeGen::emitVar(PSymbol sym, bool rvalue)
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

void CodeGen::emitVar(PSymExpr p)
{
	emitVar(p->getSymbol(), p->rvalue());
}

// size = 4 EXTENDEF
// size = 2 NORMAL
// size = 1 TOP AH
// size = -1 LOW AH
char * CodeGen::registerString(Registro reg, int size )
{
static char buf[4];
static char * regNames[] = {"eax", "ebx", "ecx", "edx", "esi", "edi", "ebp", "esp"};	
	strcpy(buf, regNames[reg]);
	if(size == 4) return buf;
	if(size == 2) return buf+1;
	if(size == 1) {
		buf[2] = 'h';
		return buf+1;
	}
	else {
		buf[2] = 'l';
		return buf+1;
	}

}

void CodeGen::genAssign(PBinExpr p)
{
	bool opt = false;
	if (*p->sx() == Nsymbol && p->sx()->lvalue()) 
		opt = true;
	else
		gen(p->sx());

	gen(p->dx());
	obuf << "mov\teax, DWORD PTR [esp]"; newline();

	if(opt) {
		obuf<< "mov\t"; 
		emitVar(PSymExpr(p->sx())->getSymbol(), true);
		obuf << ", eax"; newline();
	}
	else {
		emitPop(EBX);
		obuf << "mov DWORD PTR [ebx], eax"; newline();
	}
}

void CodeGen::genBlock(PBlock p)
{
	CBlock::iterator it = p->begin();
	while(it != p->end()) {
		gen(*it);
        ++it;
	}
}

void CodeGen::genExpr(PExprStat p)
{
	gen(p->expr);

	// stack recover if something on stack
	if(p->expr->ptipo != CType::Void()) {
		emitPop(EAX);
	}
}

// + newline
void CodeGen::emitPop(Registro reg)
{
	obuf << "pop\t" << registerString(reg, 4); newline();
}


// Versione senza ottimizzazione
// gestisce: ADD, SUB, AND, OR, XOR, 
void CodeGen::genAddSub(PBinExpr p)
{
	gen(p->sx());
	gen(p->dx());
	switch(asmTypeOf(p->ptipo)) {
	case AINT:
	case ACHAR:
	case APTR:
		emitPop(EAX);
		obuf << (*p == Nadd ? "add" : "sub") << '\t';
		obuf << "DWORD PTR [esp], eax"; newline();
		break;
	case AFLOAT:
		emitLoadFpu();
		emitPop(EAX);
		obuf << (*p == Nadd ? "fadd" : "fsub") << '\t'; 
		emitESP(); newline();
		emitStoreFpu();
		break;
	}
}

// emitstack: genera il testo "size ptr" [esp +- idx]
void CodeGen::emitESP(int idx, int size)
{
	if(size == 4)
		obuf << "DWORD PTR";
	obuf << " [esp";
	if(idx > 0) 
		obuf << "+" << dec << idx;
	else if(idx < 0)
		obuf << "-" << dec << -idx;
	obuf << "]";
}

// fld dword ptr [esp] + newline
void CodeGen::emitLoadFpu()
{
	obuf << "fld\tDWORD PTR [esp]"; newline();
}

// fstp dword ptr [esp] + newline
void CodeGen::emitStoreFpu()
{
	obuf << "fstp\tDWORD PTR [esp]";newline();
}

void CodeGen::genDeref(PUnExpr p)
{
	if (*p->sx() == Nsymbol && p->sx()->lvalue()) {
		CSymbol * ps = PSymExpr(p->sx())->getSymbol();
		if(!ps->isLocal()) {
			obuf << "push\t";
			emitVar(ps, false);
			newline();
			return ;
		}		
		
		emitVar(PSymExpr(p->sx()));
		newline();
	}
	else {
		gen(p->sx());
		emitPop(EAX);		
	}
	obuf << "push\tDWORD PTR [eax]"; newline();
}


void CodeGen::genPreIncDec(PUnExpr p)
{
	// generazione molto semplice
	gen(p->sx());
	char * is = *p == Ninc ? "inc" : "dec"; 
	switch(asmTypeOf(p->ptipo)) {
	case ACHAR:		
		emitPop(EAX);
		obuf << is << "\tbyte ptr [eax]"; newline();
		emitPush(EAX);
		break;
	case AINT:
		emitPop(EAX);
		obuf << is << "\tdword ptr [eax]"; newline();
		emitPush(EAX);
		break;
	case AFLOAT:		
		// pop eax
		// fld dword ptr [eax]
		// push 1.0f
		// fadd dword ptr [esp]
		// fstp dword ptr [eax]
		// mov dword ptr [esp], eax
		{
		float f = 1;
		int k = *((int *)&f);		// 32bit <-> 32bit
		emitPop(EAX);	// estrae indirizzo variabile
		obuf << "fld\tdword ptr [eax]"; newline();	// carica valore
		obuf << "push\t" << dec << k; newline();	// mette 1
		obuf << (*p == Nadd ? "fadd" : "fsub")
			<< "\tdword ptr [esp]"; newline(); 		
		// assegna valore del risultato
		obuf << "fstp\tdword ptr [eax]"; newline();
		// ripristina indirizzo su stack
		obuf << "mov\tdword ptr [esp], eax"; newline();
		}
		
		break;
	default:
		// TODO: error
		break;
	}
}

void CodeGen::genCompare(PRelBoolExpr p)
{
	assert(asmTypeOf(p->sx->ptipo) == asmTypeOf(p->dx->ptipo));
	gen(p->sx);
	gen(p->dx);
	AsmType t = asmTypeOf(p->sx->ptipo);
	if(t != AFLOAT) {
		emitPop(EAX);
		emitPop(EBX);
		if(t == ACHAR) 
			obuf << "cmp\tal, bl";
		else
			obuf << "cmp\teax, ebx";
		newline();
	}
	else {
		p->ldesc.floatTest = true;
		emitLoadFpu();
		emitPop(EAX);
		emitLoadFpu();
		obuf << "fcompp\t"; newline();
		obuf << "fnstsw\tax"; newline();
	}
}

// l'emissione di una label vuol dire che in questa posizione
// ognuna delle TempLabel in lset devono essere associate ad
// una data label fisica. 
// In questa implementazione LabelTemp
void CodeGen::emitLabel(LabelSet lset)
{
	unsigned j;
	lset.begin(j);
	while(lset.ok(j)) {
		int idx = lset.next(j);
		emitLabel(idx);
	}
}

void CodeGen::emitLabel(const TrueLabel lbl)
{
	obuf << "$" << dec << lbl << ":"; newline();
}

TrueLabel CodeGen::extractFirstLabel(LabelSet lset)
{
	unsigned j;
	lset.begin(j);
	if(lset.ok(j)) 
		return lset.next(j);			
	else
		return 0;
}

/*
jcond lt, L1:	test ah,1; je L1
jcond ge, L1:		test ah,1; jne L1
jcond le, L1:		test ah, 65; je L1
jcond gt, L1: 	test ah, 65; jne L1
jcond eq, L1:		test ah, 64,je L1
jcond neq, L1: 	test ah, 64; jne L1
*/
void CodeGen::emitJCond(const  LogicDesc & ld)
{
	emitJCond(ld.condizione, ld.vero, ld.floatTest);
}

void CodeGen::emitJCond(Cond c, LabelSet lset, bool isFloat )
{
char * jcondizione[] = {
		"jmp", "jmp","", "jlt", "jgt", "jle", "jge", "je", "jne"
	};
	CondType ct = c;
	
	if(ct == cFALSE) 
		return;
		
	if(!isFloat ) {
		obuf << jcondizione[(int)ct] << '\t';
		// salta alla prima di ld.vero
		obuf << "$" << extractFirstLabel(lset); newline();
	}
	else {
		// confronto
		int j = 0;
		switch(ct) {
		case cLT:
		case cGE:
			j = 0x1;
			break;
		case cGT:
		case cLE:
			j = 65;
			break;
		case cEQ:
		case cNE:
			j = 64;			
		default:
			break;
		};
		obuf << "test\tah," <<dec<< j;
		newline();
		switch(ct) {
		case cGT:
		case cGE:
		case cNE:
			obuf << "je\t";
			break;
		case cLT:
		case cLE:
		case cEQ:
			obuf << "jne\t";
			break;
		}
		obuf << "$" << extractFirstLabel(lset); 
		newline();
	}
}

void CodeGen::genOrElse1(PBoolExpr p)
{
	gen(p->sx());
	if(p->ldesc != cFALSE) 
		emitJCond(p->ldesc);
	emitLabel(p->ldesc.falso);	
}

void CodeGen::genOrElse2(PBoolExpr p)
{
	gen(p->sx());
	gen(p->dx());
}

void CodeGen::genToBool(PExpr2Bool p)
{
	gen(p->left());
	switch(asmTypeOf(p->expr->ptipo)) {
	case AINT: 
	case APTR:
		emitPop(EAX);
		obuf << "cmp\teax, 0"; newline();
		break;
	case ACHAR:
		emitPop(EAX);
		obuf <<"cmp\tal, 0"; newline();
		break;
	case AFLOAT:
		obuf << "fldz"; newline();
		emitLoadFpu();
		emitPop(EAX);
		obuf << "fcompp"; newline();
		obuf << "fnstsw\tax"; newline();
		break;
	default:
		// TODO: errore
		break;
	}
}

// expr
// * salta a falso 
// LBL: vero 
//   setta a vero
//   jmp $next
// LBL: falso
//   setta a falso
// $next
void CodeGen::genFromBool(PBool2Expr p)
{
	gen(p->left());
	LogicDesc desc = p->bexpr->ldesc;
	emitJCond(desc);
	emitLabel(desc.falso);
	obuf << "xor\tedx, edx"; newline();
	obuf << "jmp\t$tmp" << bool2exprlabelcount; newline();
	emitLabel(desc.vero);
	obuf << "mov\tedx, 1"; newline();
	obuf << "$tmp" << bool2exprlabelcount++ << ":"; newline();
	emitPush(EDX);
}

// in base al tipo di nodo genera il codice
void CodeGen::genCondIstr(PCondStat p)
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