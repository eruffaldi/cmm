// Nome: node.h
// Descrizione: nodi dell'albero di traduzione (eccetto booleani)
// Autore: Ruffaldi Emanuele
// Progetto: CPP/Base
// History: 21/1/99 added visitor pattern
#ifndef NODE_H
#define NODE_H

#include "autoptr.h"
#include "visitor.h"

enum NodeType {
	// Tipi di nodi per le dichiarazioni
	Ndecl,		// dichiarazione, binary: sx = declspec, dx = lista dirdecl
	Nargdecl,	// dichiarazione di argomento di funzione.
	Ndirdecl,	// declarator, binary: sx = declarators, dx = init list
	Nptrdecl,		// puntatore, CUnExpr
	Narraydecl,		// array, CBinExpe: sx = size expr, dx = argument
	Nfuncdecl,	// function, CBinList: sx = declarators del ritorno, dx =lista arg
	Nstatic,
	Nextern,
	Nellipse,

	// Nodi per le operazioni
	Nadd, Nsub, Nmul, Ndiv, Nmod, Nlogand, Nbinor, Nbinxor,
	Nbinand, Neq, Nneq, Nlt, Ngt, Nlteq, Ngteq, Nshl, Nshr,
	Nassign, Ncomma, Nlogor, Ncall, Ncast, Nlognot, Nbinnot, Nneg,
	Nconst,Nident,Narray,Nptr,Naddr, 

	// Istruzioni 
	Nexpr,Ndeclstat, Nfuncbody, Nblock, Nreturn, Nwhile, Ndo,
	Nif, Nfor, Nsymbol,

	// bool operations
	Norelse1, Norelse2, Nexpr2bool, Nbool2expr,Ncmp, Nboolconst,
	Ninc, Ndec,	// UnExpr
	Nincpost, Ndecpost,		// UnExpr
	Nlast
};

#include <iostream>
#include "types.h"

class CFunction;


struct lineRef {
	int _lineno, _fileno;
	lineRef(int lineno, int fileno = 0) : _fileno(fileno), _lineno(lineno) {}
	operator int () const {return _lineno;}
};

class CNode;
typedef dyn_ptr<CNode> PNode;

#include <string.h>

struct writeXmlData {
	char * source;
	writeXmlData(const char * p)
	{
		source = new char[strlen(p)+1];
		strcpy(source, p);
	}
	~writeXmlData() {delete [] source;}
};


// VISITOR PATTERN:

// Forward class declarations
class CNode;
class CUnNode;
class CExpression;
class CUnExpr;
class CBinExpr;
class CDirDecl;
class CComma;
class CDirDecl;
class CConstExpr;
class CIdentifer;
class CStatement;
class CBlock;
class CExprStat;
class CFuncBody;
class CBoolNode;
class CCondStat;
class CSymExpr;
class CBoolExpr;
class CExpr2Bool;
class CBool2Expr;
class CRelBoolExpr;

class NodeVisitor {
public:
	VISITOR_OF(CNode) = 0;
	VISITOR_OF(CUnNode) = 0;
	VISITOR_OF(CExpression) = 0;
	VISITOR_OF(CUnExpr) = 0;
	VISITOR_OF(CBinExpr) = 0;
	VISITOR_OF(CDirDecl) = 0;
	VISITOR_OF(CComma) = 0;
	VISITOR_OF(CConstExpr) = 0;
	VISITOR_OF(CIdentifer) = 0;
	VISITOR_OF(CStatement) = 0;
	VISITOR_OF(CBlock) = 0;
	VISITOR_OF(CExprStat) = 0;
	VISITOR_OF(CFuncBody) = 0;
	VISITOR_OF(CBoolNode) = 0;
	VISITOR_OF(CCondStat) = 0;
	VISITOR_OF(CSymExpr) = 0;
	VISITOR_OF(CBoolExpr) = 0;
	VISITOR_OF(CExpr2Bool) = 0;
	VISITOR_OF(CBool2Expr) = 0;
	VISITOR_OF(CRelBoolExpr) = 0;
};

// Nodo: generico
class CNode {
	NodeType nt;
public:
	CNode(NodeType n): nt(n) {}
	virtual ~CNode() {}

	virtual PNode left() const {return PNode();}
	virtual PNode right() const {return PNode();}
	virtual void setLeft(PNode p) { assert(p == 0);}
	virtual void setRight(PNode p) { assert(p == 0);}
	operator NodeType() const {return nt;}
	NodeType node() const {return nt;}
	void setNodeType(NodeType t) {nt = t;}

	// RUNTIME Type checking
	virtual bool isExpr() const {return false;}
	virtual bool isStat() const {return false;}
	virtual bool isBool() const {return false;}
	
	// Debugging 
	VISITABLE_BY(CNode, NodeVisitor);
	
	virtual void writeXml(std::ostream & o, writeXmlData&);

	static char * stringOf(NodeType op);
};


// Nodo: unary
//  generico unario
class CUnNode :public CNode {
	PNode l;
public:
	CUnNode(NodeType n, PNode pl = PNode()): CNode(n), l(pl) {}
	virtual ~CUnNode() { delete l.get();}
	
	VISITABLE_BY(CUnNode, NodeVisitor);
	virtual PNode left() const {return l;}
	virtual void setLeft(PNode p) { l = p;}
};

// Nodo: espressione
// Descrizione: fornisce le informazioni base per la manipolazione delle
//   espressioni.
class CExpression: public CNode {	
	int flag;		// lvalue = bit 0
public:
	PType  ptipo;			// tipo
	
	CExpression(NodeType n): CNode(n),ptipo(0),flag(0) {}
	virtual bool isExpr() const {return true;}
	bool lvalue() const {return (flag & 1) != 0;}
	bool rvalue() const {return (flag & 1) == 0;}
	bool cvalue() const {return  node() == Nconst;}
	void lvalue(bool b) {flag = (flag & ~1) | (b ? 1: 0);}
	VISITABLE_BY(CExpression, NodeVisitor);
	
	virtual void writeXml(std::ostream & o, writeXmlData&);
};
typedef dyn_ptr<CExpression> PExpression;

// Nodo: unary expression
// Descrizione: espressione unaria, un solo nodo figlio
//
class CUnExpr: public CExpression {
	PExpression  first;
public:
	
	CUnExpr(NodeType n, PExpression  p) : CExpression(n), first(p) {}
	
	virtual void setLeft(PNode p) { 
		assert(!p || p->isExpr());
		first = p;}
	virtual PNode left() const {return first;}
	virtual PExpression& sx() {return first;}
	VISITABLE_BY(CUnExpr, NodeVisitor);
};
typedef dyn_ptr<CUnExpr> PUnExpr;

// Nodo: binary expression
// Descrizione: memorizza un operatore binario
// 
// Questa classe e' la base di vari nodi con duplici nodi
class CBinExpr: public CUnExpr {
	PExpression  second;
public:	
	CBinExpr(NodeType n, PExpression  p1 = PExpression(),
		PExpression p2 = PExpression()) : CUnExpr(n,p1),second(p2) {}

	virtual void setRight(PNode p) { 
		assert(!p || p->isExpr());
		second = (PExpression)p;}	
	virtual PNode right() const {return second;}
	virtual PExpression& dx() {return second;}

	VISITABLE_BY(CBinExpr, NodeVisitor);
};
typedef dyn_ptr<CBinExpr> PBinExpr;

typedef dyn_ptr<CDirDecl> PDirDecl;

#include "slista.h"

// Nodo: comma expression
// Descrizione: operatore comma o altri casi a valori multipli
//
// Questo nodo viene introdotto per gestire piu' facilmente insiemi
// di sotto espressioni correlate fra di loro, ad esempio nella
// dichiarazione di piu' variabili (Ndecl), nella lista degli argomenti
// di una funzione (Nfuncdecl), nell'operatore comma (Ncomma), nei parametri
// di una chiamata a funzione (Ncall)
class CComma: public CUnExpr {
public:
	slista<PExpression> children;
	typedef slista<PExpression>::iterator iterator;

	// funzioni per generalizzare il lavoro su CComma
	iterator begin() {return children.begin();}
	iterator end() {return children.end();}
	int conta() { iterator it = begin(); int n = 0; while(it != end()) ++it,n++; return n;}
	void inverti() {children.revert();}
	CComma(NodeType nt, PExpression p = PExpression())
		: CUnExpr(nt,p) {}

	void appendChild(PExpression p) {children.push_back(p);}
	virtual void writeXml(std::ostream & o, writeXmlData&);
	VISITABLE_BY(CComma, NodeVisitor);
};
typedef dyn_ptr<CComma> PComma;

// Nodo: direct declarator 
// Descrizione: unisce identificatore all'insieme dei suoi dichiaratori
//
// Il lato sinistro di questo nodo binario contiene un ramo di dichiaratori
// Il lato destro il ramo dell'espressione di inizializzazione
class CDirDecl: public CBinExpr {
	char * ident;
	int m_lineno;
public:
	char * & identifer() {return ident;}
	void setLineNumber(int no) {m_lineno = no;}
	int lineNumber() const {return m_lineno;}
	CDirDecl(): CBinExpr(Ndirdecl) {ident = 0; m_lineno = 0;}

	// ridefinisce l'assegnamento ad sx controllando che un
	// dichiaratore oppute nulla.
	virtual void setLeft(PExpression p) 
	{
		assert(p == 0 || *p == Nptrdecl || *p == Narraydecl || *p == Nfuncdecl);
		CBinExpr::setLeft(p);
	}	
	virtual void writeXml(std::ostream & o, writeXmlData&);
	VISITABLE_BY(CDirDecl, NodeVisitor);
};


// classe: CConstExpr
// descrizione: rappresenta una espressione costante con valore
// calcolato a tempo di compilazione. Memorizza le costanti
// intere, char e floating. I costruttori di questa classe 
// assegnano il tipo di dato in automatico.
//
// fix: le stringhe vengono gestite correttamente da questa classe
class CConstExpr: public CExpression {
public:
	CConstExpr(int ival);
	CConstExpr(char cval);
	CConstExpr(float fval);
	CConstExpr(const char * str, int len);
	union {
		int ival;
		char cval;
		float fval;
		struct {
			const char * cp;
			int len;
		} strval;
	};
	 
	virtual void writeXml(std::ostream & o, writeXmlData&);
	VISITABLE_BY(CConstExpr, NodeVisitor);
};
typedef dyn_ptr<CConstExpr> PConstExpr;

class CIdentifer: public CExpression {
	char*ident;
public:
	CIdentifer(char * cp) : CExpression(Nident), ident(cp) {}
	char * & identifer() {return ident;}
	 
	virtual void writeXml(std::ostream & o, writeXmlData&);
	VISITABLE_BY(CIdentifer, NodeVisitor);
};
typedef dyn_ptr<CIdentifer> PIdentifer;

//- Statements
//***********************************************************

// Class: CStatement
// una istruzione e' caratterizzata da un riferimento alla linea
// di codice alla quale appartiene
class CStatement: public CNode {
public:
	CStatement(NodeType, lineRef);

	virtual bool isStat() const {return true;}
	virtual void writeXml(std::ostream & o, writeXmlData&);
	int lineNumber() const {return lineno._lineno;}
	int fileNumber() const {return lineno._fileno;}
	VISITABLE_BY(CStatement, NodeVisitor);
protected:
	lineRef lineno;
};
typedef dyn_ptr<CStatement> PStatement;


#include "symbol.h"

// Classe CBlock
// Questa classe svolge un ruolo fondamentale in tutta la 
// compilazione poiche' non solo rappresenta il costrutto 
// di istruzione composita, ma costituisce una parte 
// delocalizzata della tabella dei simboli
// Attributi:
// * lista delle istruzioni: children di tipo slista<PStatement>
//		( costruito durante l'analisi sintattica)
// * tabella dei simboli: table di tipo CSymbolTable
//		( costruito elaborando le dichiarazioni in an. semantica)
// * bytes richiesti di stackframe: frameSize di tipo Int
//		( costruito alla fine delle dichiarazioni in semantica)
// * collegamento al padre: parent() di tipo CBlock*
// * lista dei sottoblocchi: subBlocks di tipo slista<CBlock*> 
//		( costruita in analisi sintattica per facilitare la
//			scansione della tabella dei simboli!)
// NB: le variabili globali sono viste come istruzioni immesse dentro
// un blocco implicito, quello di visibilita' globale, l'unico
// con il parent() == 0
class CBlock: public CStatement {
public:
	// Types
	typedef slista<PStatement>::iterator iterator;
	typedef slista<CBlock*>::iterator blkIterator;


	// Attributes	
	
	// costruisce un blocco in base alla linea del file e al
	// blocco padre.
	CBlock(lineRef, CBlock*);

	virtual void writeXml(std::ostream & o, writeXmlData&);

	// Attributi
	CBlock *	parent() const {return m_parent;}	
	int			level()  const {return m_nestingLevel;}
	int			id()	 const {return m_blockID;}
	int			getFrameSize() const {return m_frameSize;}
	void		setFrameSize(int k) {m_frameSize = k;}
	iterator	begin()				{return children.begin();}
	iterator	end()					{return children.end();}
	void	appendChild(PStatement p)	{children.push_back(p);}
	blkIterator blkBegin()			{return m_blocks.begin();}
	blkIterator blkEnd()			{return m_blocks.end();}
	void appendBlock(CBlock* p)		{m_blocks.push_back(p);}
	CSymbolTable & getSymTab()		{return m_table;}
	VISITABLE_BY(CBlock, NodeVisitor);
protected:
	CBlock* m_parent;			// blocco padre
	int		m_nestingLevel;		// livello di nidificazione
	int		m_blockID;			// identificatore univoco del blocco nel file
	slista<PStatement>	children;	// istruzioni del blocco
	slista<CBlock*> m_blocks;
	CSymbolTable	m_table;
	int				m_frameSize;

	static int globalid;
};
typedef dyn_ptr<CBlock> PBlock;

// Class: CExprStat
// E' una istruzione alla quale e' associata una espressione
// ammissibili: Nreturn, Ndeclstat, Nexpr
class CExprStat: public CStatement {
public:
	
	CExprStat(NodeType,PExpression,lineRef);
	virtual PNode left() const {return expr;}

	PExpression expr;
	VISITABLE_BY(CExprStat, NodeVisitor);
};
typedef dyn_ptr<CExprStat> PExprStat;

// Class: CFuncBody
// Questa classe memorizza il body di una funzione, in particolare
// l'intestazione della funzione, il bloco di istruzioni ed il simbolo
// funzione associato.
// 
// Attributi:
//  decl	 P	dichiarazione della funzione
//	block	 P	istruzioni del body
//	function S	simbolo
class CFuncBody: public CStatement 
{
public:
	CFuncBody(int no, PBlock parent, PExpression e );
	virtual PNode left() const	{return decl;}
	virtual PNode right() const	{return block;}

	CFunction *		function;
	PExpression		decl;
	PBlock			block;
	VISITABLE_BY(CFuncBody, NodeVisitor);
};
typedef dyn_ptr<CFuncBody> PFuncBody;


class CCondStat: public CStatement {
protected:
	CCondStat(NodeType nt, int no) : CStatement(nt, no) {}
public:

	// costruisce ed inizializza un oggetto CCondStat
	// passando i possibili parametri. Per il for utilizzo una
	// funzione separata
	static CCondStat* mknode(NodeType nt, int lineno, 
		PExpression e, PStatement s1 = PStatement(), PStatement s2 = PStatement());
	static CCondStat* mknode_for(int lineno, 
		PStatement, PExpression, PExpression, PStatement);

	PExpression cond, incr;
	CBoolNode *bcond;		// condizione booleana
	PStatement block, blockII;

	// ++ Boolean Descriptor & Labels
	virtual PNode left() const {return cond;}
	virtual PNode right() const {return block;}
	VISITABLE_BY(CCondStat, NodeVisitor);
	virtual void writeXml(std::ostream & o, writeXmlData&);
};
typedef dyn_ptr<CCondStat> PCondStat;


// Classe: CSymExpr
// Espressione rappresentata da un simbolo, questa classe e' associata
// al nodo Nsymbol ed e' generata dall'analizzatore semantico. 
// L'oggetto puo' essere usato direttamente come un simbolo attraverso
// gli operatori ridefiniti
class CSymExpr : public CExpression 
{
public:	
	CSymExpr(CSymbol*);
	
	CSymbol * operator->() { return m_symbol; }
	CSymbol & operator* () { return *m_symbol;}
	CSymbol * getSymbol() const {return m_symbol;}
	virtual void writeXml(std::ostream & o, writeXmlData&){}
	VISITABLE_BY(CSymExpr, NodeVisitor);
protected:
	CSymbol * m_symbol;
};
typedef dyn_ptr<CSymExpr> PSymExpr;

#include "bools.h"
class PrintVisitor : public NodeVisitor 
{

	struct writeData {
		int level;
		int mask;
		writeData() {level = mask = 0;}
	};
	writeData level;

	enum { MASK =0x80000000};

	writeData CHILD(writeData lev) 
	{
		writeData wd;
		wd.mask = (lev.mask & ~MASK);
		wd.level = lev.level+1;
		if(!(lev.mask & MASK)) 
			wd.mask |= 1;
			wd.mask <<= 1;
		return wd;	
	}

	writeData LASTCHILD(writeData lev)
	{
		writeData wd = CHILD(lev);
		wd.mask |= MASK;
		return wd;
	}

	std::ostream & spacer(std::ostream & o, writeData level)
	{
	// 	for(int i= 0;i< n; i++) o << ' ';
		int k = level.mask & ~MASK;
		if(level.level > 0) {
		int m = 1 << (level.level-1);
		while(m > 1) {
			o << ((k & m) ? '|' : ' ');
			m >>= 1;
		}
		o << ((level.mask & MASK) ? '\\' : '|');
		o << "- ";
		}
		return o;
	}

	void doExprChild(CNode* pn);
	std::ostream & o;
	
	void stampaLogic(LogicDesc & ld);
public:
	PrintVisitor(std::ostream & _o, CNode* pn) : NodeVisitor(), o(_o) {
		level.level = level.mask = 0;
		pn->accept(*this);
	}
VISITOR_OF(CNode);
VISITOR_OF(CUnNode);
VISITOR_OF(CExpression);
VISITOR_OF(CUnExpr);
VISITOR_OF(CBinExpr);
VISITOR_OF(CDirDecl);
VISITOR_OF(CComma);
VISITOR_OF(CConstExpr);
VISITOR_OF(CIdentifer);
VISITOR_OF(CStatement);
VISITOR_OF(CBlock);
VISITOR_OF(CExprStat);
VISITOR_OF(CFuncBody);
VISITOR_OF(CBoolNode);
VISITOR_OF(CCondStat);
VISITOR_OF(CSymExpr);
VISITOR_OF(CBoolExpr);
VISITOR_OF(CExpr2Bool);
VISITOR_OF(CBool2Expr);
VISITOR_OF(CRelBoolExpr);
};

#endif

