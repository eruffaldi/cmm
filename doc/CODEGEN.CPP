// Funzione di generazione del codice

void genConst(PConstExpr p)
{
	emitPush();
	emitConst(p);
}

void genSymbol(PSymExpr p)
{	
	if (p->lvalue()) {
		if(p->sym()->isLocal()) {
			emit("lea eax, ");
			emitVar(p);
			newline();
			emitPush(EAX);
		}
		else {
			emit("push ");
			emitVar(p);
		}
	}
	else
		emitPush(p->sym());
	newline();
}

void genDeref(PUnExpr p)
{
	if (p->e1()->id() == Nsymbol && p->e1()->rvalue()) {
		emit("mov eax, ");
		emitVar(p->e1());
		newline();
	}
	else {
		gen(p->e1());
		emit("pop eax");
		newline();
	}
	emit("push dword ptr [eax]");
	newline();
}

void genConvert(PUnExpr p)
{
	AsmType src,dst;	
	gen(p->e1());
	src = asmtypeof(p);
	dst = asmtypeof(p->e1());
	if(src == ACHAR) {
		switch(dst) {
		case AINT:
		case APTR:
			emit("pop eax"); newline();
			emit("movsx eax, al"); newline();
			emitPush(EAX);
			break;
		case AFLOAT:
			emit("pop eax"); newline();
			emit("movsx eax, al"); newline();
			emitPush(EAX);
			emit("fild dword ptr [esp]"); newline();
			emitStoreFpu();
			break;
		default:
			// TODO: error
			break;
		}
	}
	else if(src == AINT) {
		switch(dst) {
		case ACHAR: 
		case APTR: break;
		case AFLOAT:
			emit("fild dword ptr [esp]"); newline();
			emitStoreFpu();
			break;
		default:
			// TODO: errore
		}
	}
	else if(src == AFLOAT) {
		switch(dst) {
		case AINT: 
			// TODO: dichiara la funzione __ftol
			emitLoadFpu();
			emit("call __ftol");
			newline;
			emitStoreFpu();
			break;
		default:
			// TODO: errore
			break;
		}
	}
	else if(src == APTR && dst != AINT)
		// TODO: errore
		;
	else
		// TODO: errore
		;
}

void genNegate(PUnExpr p)
{
	gen(p->e1());
	switch(asmtypeof(p)) {
	case ACHAR:		
		emit("neg byte ptr [esp]"); newline();
		break;
	case AINT:
		emit("neg dword ptr [esp]");
		break;
	case AFLOAT:
		emitLoadFpu();
		emit("fchs"); newline();
		emitStoreFpu();
		break;
	default:
		// TODO: error
		break;
	}
}

void genBinNot(PUnExpr p)
{
	gen(p->e1());
	switch(asmtypeof(p)) {
	case ACHAR:
	case AINT:
		emit("neg dword ptr [esp]"); newline();
		break;
	default:
		// TODO: error
		break;
	}
}

// Versione senza ottimizzazione
// gestisce: ADD, SUB, AND, OR, XOR, 
void genAddSub(PBinExpr p)
{
	gen(p->e1());
	gen(p->e2());
	switch(asmtypeof(p)) {
	case AINT:
	case ACHAR:
	case APTR:
		emitPop(EAX)
		emit(p->id() == Nadd ? "add" : "sub");
		emit(" dword ptr [esp], eax"); newline();
		break;
	case AFLOAT:
		emitLoadFpu();
		emitPop(EAX);
		emit(p->id() == Nadd ? "faddp" : "fsubp");
		emitESP();
		emitStoreFpu();
		break;
	}
}


void genCompare(PRelExpr p)
{
	AsmType t = asmtypeof(p);
	assert(asmtypeof(p->e1()) == asmtypeof(p->e2()));
	gen(p->e1());
	gen(p->e2());
	if(t != AFLOAT) {
		emitPop(EAX);
		emitPop(EBX);
		if(t == ACHAR) 
			emit("cmp al, bl");		
		else
			emit("cmp eax, ebx");
		newline();
	}
	else {
		emitLoadFpu();
		emitPop();
		emitLoadFpu();
		emit("fcompp"); newline();
		emit("fnstsw ax"); newline();
	}
}

void genOrElse1(PUnBoolExpr p)
{
	gen(p->e1());
	if(p->desc.cond != condFalse) 
		emitJCond(p->desc);
	emitLabel(p->desc.labelFalse);	
}

void genOrElse2(PBinBoolExpr p)
{
	gen(p->e1());
	gen(p->e2());
	// donothing
}

void genAssign(PBinExpr p)
{
bool opt = false;
	if (p->e1()->id() == Nsymbol && p->e1()->lvalue()) 
		opt = true;
	else
		gen(p->e1());

	gen(p->e2());
	obuf << "mov eax, dword ptr [esp]"; 
newline();

	char * sz = (asmtypeof(p) == ACHAR) ? "byte" : "dword";
	
	if(opt) {
		emit("mov "); emitVar(PSymExpr(p->e1)->sym(),true);
		emit(", eax"); newline();
	}
	else {
		emitPop(EBX);
		emit("mov dword ptr [ebx], eax");
	}
}

void genCallExpr(PCallExpr p)
{
	// TODO: ricava il tipo di funzione: cdecl o stdcall
	bool cdecl = true;
	int size = 0;
	if(cdecl) {
		// TODO: per tutti gli argomenti all'incontrario
		gen(e);
		// TODO: conta per ogni e size
	}
	else {
		// TODO: per tutti gli argomenti in avanti
		gen(e);
	}
	
	if(p->sym() != 0) {
		emit("call "); 
		if(!p->sym() /* TODO: e' una funzione*/) 
			emitFunc(p->sym()); 
		else
			emitVar(p->sym());
		newline();
	}
	else {
		gen(p->efunc);
		emitPop();
		emit("call dword ptr [eax]"); newline();
	}

	// ripulisce 
	if(cdecl) 
		emit("add esp, "); emit(size); newline();	
	AsmType t = asmtypeof(p);
	if(t == AFLOAT) {
		emitPush();
		emitStoreFpu();
	}
	else if(t != AVOID) 
		emitPush();	// risultato in EAX	
}

void genToBool(PToBoolExpr p)
{
	assert(p->e1()->isArith);
	gen(p->e1());
	switch(asmtypeof(p)) {
	case AINT: 
	case APTR:
		emitPop();
		emit("cmp eax, 0"); newline();
		break;
	case ACHAR:
		emitPop();
		emit("cmp al, 0"); newline();
		break;
	case AFLOAT:
		emit("fldz"); newline();
		emitLoadFpu();
		emitPop();
		emit("fcompp"); newline();
		emit("fnstsw ax"); newline();
		break;
	default:
		// TODO: errore
		break;
	}
}

void genFromBool(PFromBoolExpr p)
{
	assert(p->e1()->isBoolean());
	gen(p->e1());
	CLogicDesc desc = p->desc();
	desc.cond.not();
	emit("xor edx, edx"); newline();
	emitJCond(desc);
	emitLabel(desc.labeltrue);
	emit("inc edx"); newline();
	emitLabel(desc.labelfalse);
	emitPush(EDX);
}

void genCondIstr(PCondStatement p)
{
	// TODO: inizializzazione di array!
}

void gen(PNode p) 
{
	switch (p->id()) {
		case Nconst: genConst(PConstExpr(p));	break;
		case Nsymbol: genSymbol(PSymExpr(p));	break;
		case Nderef: genDeref(PUnExpr(p));		break;
		case Nconvert: genConvert(PUnExpr(p));	break;
		case Nnegate: genNegate(PUnExpr(p));	break;
		case Nbinnot: genBinNot(PUnExpr(p));	break;
		case Nadd:
		case Nsub:	genAddSub(PBinExpr(p));		break;
		case Nmul: genMul(PBinExpr(p)); break;
		case Ndiv: 
		case Nmod: genDivMod(PBinExpr(p)); break;
		case Nand:
		case Nor:
		case Nxor:  genBinary(PBinExpr(p));	break;
		case Nshl:
		case Nshr:	genShift(PBinExpr(p)); break;
		case Ncompare: genCompare(PRelExpr(p)); break;
		case Norelse1: genOrElse1(PUnBoolExpr(p)); break;
		case Norelse2: genOrElse2(PBinBoolExpr(p)); break;
		case Nassign: genAssign(PBinExpr(p)); break;
		case Nifelse:
		case Nwhile:
		case Nfor:
		case Ndowhile:	genCondIstr(PCondStatement(p)); break;
		case Ncall:		genCallExpr(PCallExpr(p)); break;
		case Ntobool:	genToBool(PToBoolExpr(p)); break;
		case Ninitvar:  genInitVar(PInitStatement(p)); break;
		case Nfrombool: genFromBool(PFromBoolExpr(p)); break;
		case Ncomma:    genComma(PCommaExpr(p)); break;
		default:
			// nodo semantico non riconosciuto
	}	
}

