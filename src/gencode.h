// Nome: gencode.h
// Progetto: CPP/Generator
// Autore: Ruffaldi Emanuele
// Generatore di codice per Intel x86
#ifndef GENCODE_H
#define GENCODE_H

#include "node.h"
#include <fstream>
#ifdef _MSC_VER
#include <strstrea.h>
#else
#include <strstream>
#endif

#include "bools.h"
#include "mappa.h"
#include "dvector.h"


class CodeGen;
typedef void (CodeGen::* symbolIterator)(CSymbol *);

// Code Generator
class CodeGen {	
protected:
	enum Registro {EAX, EBX, ECX, EDX, ESI, EDI, EBP, ESP};
	enum AsmType { AVOID, ACHAR, AINT, APTR, AFLOAT};

public:
	CodeGen(char * source, char * output, PBlock top);
	bool ready(); 
private:
	// members
	slista<CSymbol*> symEsterni;	// solo se usate
	slista<CFunction *> funDefinite;	
	slista<CSymbol *> globalVar;
	dvector<CConstExpr*> stringStorage;	
		
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
		void processLocalBlock(PBlock pb, int startoff, bool topmost = false);	
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
	void genExpr(PExprStat);
	void genBlock(PBlock p);
	void genAddSub(PBinExpr);
	void genDeref(PUnExpr);
	void genCall(PBinExpr);
	void genCast(PUnExpr);
	void genReturn(PExprStat);
	void genNegate(PUnExpr);
	void genBinNot(PUnExpr);
	void genBinary(PBinExpr);
	void genMul(PBinExpr);
	void genDivMod(PBinExpr);
	void genShift(PBinExpr);
	void genComma(PComma);
	void genPreIncDec(PUnExpr);


	void emitESP(int idx = 0, int size = 4);
	void emitLoadFpu();		// fld dword ptr [esp] + newline
	void emitStoreFpu();	// fstp dword ptr [esp] + newline

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

	const char * registerString(Registro reg, int size = 4);
	void emitPop(Registro reg); // + newline
	void emitPush(Registro reg);	// push del registro...+ newline
	// la emitPush di un simbolo genera:
	//	push + emitVar(symbol)
	// sempre come rvalue
	void emitPush(PSymbol);	// +newline
	// emitconst: genera il testo della costante, se e' float o string alloca
	//	 anche un simbolo.
	void emitConst(PConstExpr p);

	// Utility Functions
	// emitvar: genera il testo corrispondende al simbolo specificato
	//	 se c'e il flag lvalue ed e' statica o globale o extern premette
	//		"offset"
	//	 altrimenti "size ptr" dove size puo' essere byte o dword	
	// La versione overloaded controlla che il nodo sia di tipo Nsymbol e
	// richiama emitvar passando rvalue secondo l'informazione nel nodo
	void emitVar(PSymbol sym, bool rvalue = true);
	void emitVar(PSymExpr p);

};
#endif