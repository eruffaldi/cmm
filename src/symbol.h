// Nome: symbol
// Progetto: CPP/Base
// Autore: Ruffaldi Emanuele
// Descrizione: dichiarazione dei simboli
#ifndef SYMBOL_H
#define SYMBOL_H


#include "types.h"
#include "htable.h"
#include "pair.h"
#include "node.h"

class CSymbol;
typedef CSymbol * PSymbol;

typedef HashishTable<char *, PSymbol, same<char *> > CSymbolTable;

enum StorageType { stoNORMAL, stoEXTERN, stoSTATIC, stoPARAM};

//- Classe che memorizza un simbolo del programma
class CSymbol
{
	friend class Semantic;
public:
	CSymbol(char *ident, PType typo, int lin, CSymbol * padre) : 
	  nome(ident), ptipo(typo), linea(lin) {
		  m_kind = stoNORMAL;used = inited = 0; codice = 0;
		  asmName = 0;
		  m_parent = padre;
	 }

	 ~CSymbol() {delete[] asmName;}
	
	CNode* codice;	
	bool used;			// usata
	bool inited;		// inizializzata
	
	// Proprieta'
	char * getNome() const		{return nome;}	
	PType  getTipo() const		{return ptipo;}
	bool isExtern() const		{return m_kind == stoEXTERN ;}
	bool isParam() const {return m_kind == stoPARAM;}
	bool isStatic() const {return m_kind == stoSTATIC;}
	bool isLocal() const {return (m_kind == stoNORMAL && parent() != 0)|| isParam();}

	int getLineNumber() const	{return linea;}
	StorageType sType() const {return m_kind;}
	void setSType(StorageType kd) {
		// if(isFunction()) assert(kd == stoEXTERN || kd == stoNORMAL);
		m_kind = kd; 
	}
	virtual bool isFunction()	{return false;}
	CSymbol * parent() const {return m_parent;}

	// ASSEMBLER STORAGE INFO: calculated by GEN
	int		offset;			
	int 	index;
	char*   asmName;
protected:
	char *  nome;
	PType	ptipo;
	int		linea;
	StorageType m_kind;
	CSymbol * m_parent;
	
};

//- CStruct structured type
class CStruct: public CSymbol
{
public:
	CStruct::CStruct(const char* name, int lineno, CSymbol * padre);
	~CStruct();

	// aggiunge un elemento alla struttura 
	int addChild(const char * nome, PType tipo, int lineno, int packing = 0); 
	int offsetOf(const char * nome);
	PType typeOf(const char * nome);
	int size() const {return m_size;}

	CSymbolTable	table;
protected:
	int	 m_size;
	

	CSymbol * findChild(const char * nome);
};

//- CFunction, memorizza una funzione
class CFunction: public CSymbol
{
	friend class Semantic;
	bool fLeaf;
public:
	bool proto;
	int frameSize;		// dimensione del frame
	int argSize;		// dimensione degli argomenti in byte
	
	

	virtual bool isFunction()	{return true;}
	CStatement* getBody() const {return (CStatement*)(codice);}
	CSymbolTable & getTable();
	bool isLeaf() {return fLeaf;}
	CFunction(char * ident, PType pt, int linea, CSymbol * p):
		CSymbol(ident, pt, linea,p), frameSize(0) { argSize = frameSize = 0;}

	void functionHasCalledSomeone() {fLeaf = true;}
};

typedef CFunction * PFunction;

#endif
