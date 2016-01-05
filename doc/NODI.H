// Tipi di nodi dell'analisi semantica
// pseudo codice

// enumerati dell'analisi semantica
Nconst,		// CConstExpr
Nsymbol,	// CSymExpr
Nderef,		// CUnExpr
Nconvert,	// "" 
Nnegate,	// ""
Nbinnot,	// ""
Nadd,		// CBinExpr
Nsub,		// ""
Nmul,		// ""
Ndiv,		// ""
Nmod,		// ""
Nand,		// ""
Nor,		// ""
Nxor,		// ""
Nshl,		// ""
Nshr,		// ""
Ncompare,	// CRelExpr
Norelse1,	// CUnBoolExpr
Norelse2,	// CBinBoolExpr
Nassign,	// CBinExpr
Nifelse,	// CCondStatement
Nwhile,		// ""
Ndowhile,	// ""
Nfor,		// ""
Nbreak,		// CStatement
Ncontinue,	// CStatement 
Nexpr,		// ""
Ncall,		// CCallExpr
Ntobool,	// CToBoolExpr
Nfrombool,	// CFromBoolExpr
Ninitvar,   // CInitStatement
Ncomma,		// CArithExpr

class CBoolExpr;
class CArithExpr;
class CLogicDescr;
class CCondition;


class CNode
	enumerato id() const;

// istruzione generale
class CStatement: CNode
	linenumber

class CBlockStatement: CStatement
	lista di CStatement
	livello di nesting del blocco

// istruzione di tipo espressione aritmetica
class CExprStatement: CStatement
	CArithExpr *


// inizializzazione di variabili statiche
// in futuro init anche di array!
class CIniStatement: CExprStatement
	CArithExpr * 
	CSymbol
	int StaticCount

// istruzione di tipo condizionale: if, for, while, do..while
class CCondStatement: CStatement
	CBoolExpr *
	CArithExpr *				// incremento del for 
	CBlockStatement b1, b2;		// b2 serve solo per l'if
	CLogicDescr					// descrittore logico
	CLabel vero,falso, unisci;

class CExpression: CNodo

class CBoolExpr : CExpression
	CLogicDescr	desc
	desc()

// espressione di confronto, ovvero < > = ...
class CRelExpr: CBoolExpr
	CArithExpr * p1, *p2
	CCondition

// espressione unaria logica orelse1 (esiste anche andelse1 ma viene
// trasformata in orelse1, stessa cosa per il not)
class CUnBoolExpr: CBoolExpr
	CBoolExpr * p1;
	


class CBinBoolExpr: CBoolExpr
	CBoolExpr * p1, *p2;

// passaggio da arithmetico a booleano
class CToBoolExpr: CBoolExpr
	CArithExpr *

class CArithExpr: CExpression
	lvalue
	tipo
	bool lvalue() const; 
	bool rvalue() const;

class CConstExpr: CArithExpr
	valore della costante: float, char, int , indice nella tabella stringhe

class CSymExpr: CArithExpr
	simbolo

// chiamata a funzione con tutti i parametri
class CCallExpr: CArithExpr
	CArithExpr * efunc;
	CCommaExpr   parametri;
	PSymbol  sym()							// se c'e' simbolo ignora funzione

// operatori unari: *, &, cv, binnot, neg, rvalue
class CUnExpr: CArithExpr
	CArithExpr * 
	e1()

// gran parte degli operatori
class CBinExpr: CArithExpr
	CArithExpr *p1, *p2
	e1(), e2()
// converte unaespressione booleana in un intero 0 o 1
class CFromBoolExpr: CUnExpr
	CBoolExpr* p
	CLabel vero, falso


class CCommaExpr: CArithExpr 
	lista di CArithExpr

// NODI SOLO PER L'analisi sintattica
class CIdentifer: CNode
	identificatore

class CDecl: CUnExpr
	identificatore	

class CArrayExpr: CBinExpr
	CArithExpr * dimensione;	// vuoto se []

class CPrimTypeNode: CNode
	tipo primitivo: float, void, int, char

class CCastExpr: CUnExpr
	CDecl*

class CodeGen {
		
	enum Registro {EAX, EBX, ECX, EDX, ESI, EDI, EBP, ESP};
	enum AsmType { AVOID, ACHAR, AINT, APTR, AFLOAT};

	void gen(PNode);
	void genConst(PConstExpr);
	void genSymbol(PSymExpr);
	void genConvert(PUnExpr);
	void genDeref(PUnExpr);
	void genNegate(PUnExpr);
	void genBinNot(PUnExpr);
	void genBinary(PBinExpr);
	void genMul(PBinExpr);
	void genDivMod(PBinExpr);
	void genShift(PBinExpr);
	void genCompare(PRelExpr);
	void genOrElse2(PBinBoolExpr);
	void genOrElse1(PUnBoolExpr);
	void genAssign(PBinExpr);
	void genCondIstr(PCondStatement);
	void genCallExpr(PCallExpr);
	void genToBool(PToBoolExpr);
	void genFromBool(PFromBoolExpr);
	void genComma(PCommaExpr);

	void addComment(char *s);		// commento alla linea corrente
	void newline();
	// Utility Functions
	// emitvar: genera il testo corrispondende al simbolo specificato
	//	 se c'e il flag lvalue ed e' statica o globale o extern premette
	//		"offset"
	//	 altrimenti "size ptr" dove size puo' essere byte o dword	
	// La versione overloaded controlla che il nodo sia di tipo Nsymbol e
	// richiama emitvar passando rvalue secondo l'informazione nel nodo
	void emitVar(PSymbol sym, bool rvalue = true);
	void emitVar(PNode p);
	
	// emitconst: genera il testo della costante, se e' float o string alloca
	//	 anche un simbolo.
	void emitConst(PConstExpr p);
	
	// emitfunc: genera il testo mangled della funzione e aggiunge un
	// commento al nome originale.
	void emitFunc(PFunction fun);
	
	// emitstack: genera il testo "size ptr" [esp +- idx]
	void emitESP(int idx = 0; int size = 4);
	
	void emitLoadFpu();		// fld dword ptr [esp] + newline
	void emitStoreFpu();	// fstp dword ptr [esp] + newline
	void emitLabel(CLabel lbl);		// $lbl.num:
	void emitJCond(const CLogicDescr &);	// vedere manuale
	void emitPush(Registro reg);	// push del registro...+ newline
	// la emitPush di un simbolo genera:
	//	push + emitVar(symbol)
	// sempre come rvalue
	void emitPush(PSymbol);	// +newline
	void emitPop(Registro reg); // + newline
	void emit(char *);		// emette direttamente il testo
	void emit(int);
	AsmType asmtypeof(PArithExpr);
};