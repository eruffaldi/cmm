// Nome: axpgen
// Descrizione: generator ecodice Alpha
// Autore: Ruffaldi Emanuele
// Progetto: CPP/codegen
//
// AXP Code Generator for C+- Compiler 
// by Ruffaldi Emanuele
//
// Porting from x86 Release to Alpha Release
//
// Description: this code generator produces bad code for the
// Alpha processor through using this processor like a stack
// machine. Il codice viene generato tutto per operazioni INT e
// FLOAT, 
//
// Specifiche dell'architettura:
// float = 32bit	(IEEE 32bit)
// ptr = 64bit	(quadword)
// int = 32bit	(longword)
//
// Lo stack e' allineato ai 64bit
// 
// Registri usati:
// 1) quelli per i primi 6 argomenti se la fx e' leaf
// 2) 0 per il ritorno
// 3) 0-1 float e int per il caloco push/pop
// 4) s0 per il puntatore al mio stack fittizio
//
// Function Call:
// * non esiste cdecl
// * metti i primi sei nei registri
// * chiamata
// * ripristino GOT
// 
// TODO LIST:
// genCompare
// emitJCond
// genToBool 
// genFromBool
// genConst.AFLOAT
// genCall
// genReturn jmp
// genCast
// modifcare smistamento switch per unificazione genBinExp
// genNot
// cambiata semantica di offset per le variabili:
//   param = 24 + index * 8		index <= 6
//	 param = (-(paramcount-6+index+ 
//
// Struttura dello stack nella funzione:
// MYSTACK (ZONA 3)
// saved ra
// saved fp
// saved s0
// variabili locali. (ZONA 1)
// copia degli argomenti indice <= 6 ZONA2)
// argomenti extra passati dal chiamante
//
// La posizione di stack e' scelta per facilitare la memorizzazione
// dei parametri > 6 nella chiamata (se non usassi MYSTACK quella zona
// deve contenere il numero massimo di argomenti extra per le chiamate)
//
// * calcolo dello spazio per il mio stack (getStackRequest) ZONA 3
// * calcolo degli offset delle variabili locali => size di ZONA1
//   uso il campo framesize del simbolo della funzione
// * calcolo dello spazio per i primi 6 argomenti => size di ZONA2
//   min(6, argcount)*8
// 
// 
#ifndef AXPGEN_H
#define AXPGEN_H

#include "node.h"
#include <fstream>
#ifdef _MSC_VER
#include <strstrea.h>
#else
#include <strstream>
#endif

#include "bools.h"
#include "mappa.h"



// AXP Code Generator
class AXPGen {	
protected:
	enum AsmType { AVOID, ACHAR, AINT, APTR, AFLOAT};
	enum { FIRSTLOCALVAR = 24};	

	// AXP Register Identifers
	enum { r0, rT0 , rT1, rT2, rT3, rT4, rT5, rT6, rT7,
		rS0  , rS1, rS2, rS3, rS4, rS5,
		rFP , rA0 , rA1, rA2, rA3, rA4, rA5,
	tT8 , rT9, tT10, rT11, rRA , rPV ,rAT,
	rGP, rSP, rZERO};
	typedef int Registro;

	struct memory {
		int index;
		int		 offset;
		memory(int r, int o = 0) : index(r), offset(o) {}
	};
	struct imm {
		int immed;
		imm(int k) :immed(k) {}
		operator int () const {return immed;}
	};

	struct stackframeinfo {		
		int offLocalVar;	// questi sono offset che partono da
							// FIRSTLOCALVAR
		int cntLocalVar;
		int cntSavedParams;	
		int offSavedParams;
		int cntStackSize;
		int extraParams;
		int size;		
	};

	int exitlabelcount;
	stackframeinfo frame;
public:
	AXPGen(const char * source, const char * output, PBlock top);
	bool ready(); 
private:
	// members
	slista<CSymbol*> symEsterni;	// solo se usate
	slista<CFunction *> funDefinite;	
	slista<CSymbol *> globalVar;
		
	bool ftoldeclared;
	char outbuf[255];
	char comment[255];
	int bool2exprlabelcount;
	std::ofstream out;
	std::ostrstream obuf, ofunctionCode, ocomment;
	PBlock m_top;
	bool inCode;

	// premessa e finale del file, indipendenti da ogni cosa
	void preCode();
	void postCode();
	void declareFTOL() {ftoldeclared = true;}
	char * mangleSymbol(CSymbol * );

	void prolog(CFunction *);
	void epilog(CFunction *);
	void emitSymOffset(CSymbol*);

	// selezione e stampa dei simboli
	void printPublicVar();
	void definePublicVar();
	void printPublicFun();
	void printExternSym();
	void processFunctions();

	void processLocalSymbols(CFunction *);
		void processLocalBlock(PBlock pb, int& startoff, bool topmost = false);	
	void processSymbols(PBlock pb, int where);
	

	// il testo della linea corrente viene generato in un buffer
	// temporaneo, obuf e poi emesso insieme ad un commento
	void addComment(char *s);		// commento alla linea corrente
	void newline();

	AsmType asmTypeOf(PType);
	int fullSizeOf(PType);
	int argSizeOf(PType);
	

	
	void gen(PNode);
	void genConst(PConstExpr);
	void genSymbol(PSymExpr);
	void genAssign(PBinExpr);
	void genExpr(PExprStat p);
	void genBlock(PBlock p);
	void genDeref(PUnExpr);
	void genCall(PBinExpr);
	void genCast(PUnExpr);
	void genReturn(PExprStat);
	void genBinOp(PBinExpr);
	void genNegate(PUnExpr);
	void genBinNot(PUnExpr);
	void genBinary(PBinExpr);
	void genComma(PComma);


	void genCompare(PRelBoolExpr);
	void genOrElse2(PBoolExpr);
	void genOrElse1(PBoolExpr);
	
	void genCondIstr(PCondStat);
	void genToBool(PExpr2Bool);
	void genFromBool(PBool2Expr);

	void emitLabel(TrueLabel lbl);		
	void emitLabel(LabelSet lbl);		
	void emitJCond(const LogicDesc& ld);	// vedere manuale	
	void emitJCond(Cond c, LabelSet lset, bool isFloat = false);	// vedere manuale	
	TrueLabel extractFirstLabel(LabelSet lb);

	char *makeTypedIstr(const char *src, AsmType t);
/*
	
	
	void genComma(PCommaExpr);


	
	
	
	// emitfunc: genera il testo mangled della funzione e aggiunge un
	// commento al nome originale.
	void emitFunc(PFunction fun);
	
	

*/	
	const char * registerString(Registro reg, bool isFloat = false);
	void emitPop(Registro reg, AsmType t = AINT); 
	void emitPush(Registro reg, AsmType t = AINT);	
	void emitTop(Registro reg, AsmType t = AINT);

	void emitCharLoad(Registro addr, Registro tmp, Registro dst);
	void emitCharStore(Registro addr, Registro tmp, Registro src);
	void emitStore(Registro addr, Registro tmp, Registro src, AsmType t);	
	void emitIstr(const char * text, Registro r,memory mem, bool isFloat = false);
	void emitIstr(const char * text, Registro r,PSymbol, bool isFloat = false);
	void emitIstr(const char * text, Registro r_src, Registro r_src2, Registro r_dst, bool isFloat = false);
	void emitIstr(const char * text, Registro r_src, imm immed, Registro r_dst);
	void emitIstr(const char * text, Registro r_src, Registro r_dst, bool isFloat = false);
	void emitIstr(const char * text, imm immed, Registro r_dst);

	// emitconst: genera il testo della costante, se e' float o string alloca
	//	 anche un simbolo.
	void emitConst(PConstExpr);


	int getStackRequest(PNode);

	// Utility Functions
	// emitvar: genera il testo corrispondende al simbolo specificato
	//	 se c'e il flag lvalue ed e' statica o globale o extern premette
	//		"offset"
	//	 altrimenti "size ptr" dove size puo' essere byte o dword	
	// La versione overloaded controlla che il nodo sia di tipo Nsymbol e
	// richiama emitvar passando rvalue secondo l'informazione nel nodo
	void emitVar(PSymbol sym, bool rvalue = true);
	void emitVar(PSymExpr);

};
#endif
