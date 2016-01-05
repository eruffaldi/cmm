#include "node.h"
#include "semantic.h"

CSymbolTable& CFunction::getTable() {return PBlock(codice)->getSymTab();}


CStruct::CStruct(const char* name, int lineno, CSymbol * padre):
	CSymbol((char*)name, 0,  lineno, padre)
{
	m_size = 0;
	ptipo = CType::structured(this);
}

CStruct::~CStruct()
{
	CSymbolTable::iterator it = table.begin();
	for(;it != table.end(); ++it)
		delete *it;
}

int CStruct::addChild(const char * nome, PType ptipo, int lineno, int packing )
{
	if(findChild(nome)) return -1;
	CSymbol * psym = new CSymbol((char*)nome,ptipo, lineno, this);
	psym->offset = m_size;
	m_size += TypeCheck::typeSize(ptipo);
	table.put((char*)nome, psym);
	return psym->offset;
}

int CStruct::offsetOf(const char * nome)
{
	CSymbol * psym;
	if(!(psym = findChild(nome))) return -1;
	return psym->offset;
}

PType CStruct::typeOf(const char * nome)
{
	CSymbol * psym;
	if(!(psym = findChild(nome))) return 0;
	return psym->getTipo();
}


CSymbol * CStruct::findChild(const char * nome)
{

	return table.get((char*)nome);
	
}