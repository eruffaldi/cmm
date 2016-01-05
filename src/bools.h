// Nome: bools.h
// Descrizione: nodi booleani dell'albero di traduzione 
// Autore: Ruffaldi Emanuele
// Progetto: CPP/Base
#ifndef BOOLS_H
#define BOOLS_H

// il CondType specifica il tipo di confronto da effettuare
enum CondType {cNIL, cTRUE, cFALSE, cLT, cGT, cLE, cGE, cEQ, cNE};

// classe: Cond
// questa classe incapsula un CondType e fornisce le operazioni per negare un
// CondType
class Cond {
public:
	Cond(CondType t) : ct(t) {}
	static CondType tbl[9];
	void not() {
		
		ct = tbl[(int)ct];
	}

	operator CondType () const {return ct;}
	Cond operator! () const {Cond c = *this; c.not(); return c;}
private:
	CondType ct;
};

#include "NatSet.h"
typedef int TempLabel;
typedef int TrueLabel;
typedef NatSet LabelSet;

// classe: LogicDesc
// Un descrittore logico memorizza le informazioni delle operazioni booleane
// < condizione, label vera, label falsa, label unisci > 
// il falg floatTest e' necessario per una corretta implementazione dei 
// confronti in fase di generazione. 
class LogicDesc {
public:
	LogicDesc(CondType t = cNIL, LabelSet v = LabelSet(), 
		LabelSet f = LabelSet()) : condizione(t),
		vero(v), falso(f), floatTest(false) {}
	Cond condizione;
	LabelSet  vero;
	LabelSet  falso;
	LabelSet  unisci;	
	operator CondType  () const {return CondType(condizione);}
	bool	  floatTest;	

	void not() {
		condizione.not();
		LabelSet tmp = vero;
		vero = falso;
		falso = tmp;
	}

	
};


// New Boolean Node: orelse not
class CBoolNode : public CExpression
{
public:
	CBoolNode(NodeType nt) : CExpression(nt) {}
	virtual bool isBool() const {return true;}
	// TODO: virtual void writeXml(ostream & o, writeXmlData&);
	bool isConst() const {return ldesc.condizione == cFALSE || ldesc.condizione == cTRUE;}
	LogicDesc ldesc;
	VISITABLE_BY(CBoolNode, NodeVisitor);
};

typedef dyn_ptr<CBoolNode> PBoolNode;

class CBoolExpr : public CBoolNode {
public:
	CBoolExpr(NodeType nt, PBoolNode p1 , PBoolNode p2 = PBoolNode())
		:	CBoolNode(nt) , s(p1), d(p2) {}
	virtual PNode left()  const {return s;}
	virtual PNode right()  const {return d;}
	virtual PBoolNode & sx() { return s;}
	virtual PBoolNode & dx() { return d;}
	VISITABLE_BY(CBoolExpr, NodeVisitor);
private:
	PBoolNode  s, d;
};
typedef dyn_ptr<CBoolExpr> PBoolExpr;

class CExpr2Bool : public CBoolNode {
public:
	CExpr2Bool(PExpression p) :CBoolNode(Nexpr2bool) , expr(p) {}
		
	PExpression  expr;
	virtual PNode left()  const {return expr;}
	virtual void setLeft(PNode p) { expr = p;}
	VISITABLE_BY(CExpr2Bool, NodeVisitor);
};
typedef dyn_ptr<CExpr2Bool> PExpr2Bool;

class CBool2Expr: public CExpression
{
public:
	
	CBool2Expr(CBoolNode * p): CExpression(Nbool2expr), bexpr(p) {}
	virtual PNode left() const  {return bexpr;}
	VISITABLE_BY(CBool2Expr, NodeVisitor);
public:
	PBoolNode bexpr;
};
typedef dyn_ptr<CBool2Expr> PBool2Expr;

class CRelBoolExpr: public CBoolNode {
public:
	CRelBoolExpr(CExpression*s, CExpression*d, CondType ct) : CBoolNode(Ncmp), 
		sx(s), dx(d) {
		ldesc.condizione = Cond(ct);
	}
	virtual PNode left()  const {return sx;}
	virtual PNode right()  const {return dx;}
	PExpression sx,dx;
	VISITABLE_BY(CRelBoolExpr, NodeVisitor);
};

typedef dyn_ptr<CRelBoolExpr> PRelBoolExpr;

#endif
