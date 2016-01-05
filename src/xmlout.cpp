// Nome: xmlout.h
// Descrizione: stampa in XML dell'albero
// Autore: Ruffaldi Emanuele
// Progetto: CPP/Base
#include <iostream.h>
#include <strstrea.h>
#include "node.h"

char * tagNode [] = 
{
	"DECL",		// dichiarazione, binary: sx = declspec, dx = lista dirdecl
	"ARGDECL",	// dichiarazione di argomento di funzione.
	"decl:dir",	// declarator, binary: sx = declarators, dx = init list
	"PTRDECL",		// puntatore, CUnExpr
	"ARRAYDECL",		// array, CBinExpe: sx = size expr, dx = argument
	"FUNCDECL",	// function, CBinList: sx = declarators del ritorno, dx =lista arg
	"STATIC",
	"EXTERN",
	"ELLIPSE",
	"ADD", "SUB", "MUL", "DIV", "MOD", "LAND", "OR", "XOR",
	"AND", "EQ", "NEQ", "LT", "GT", "LTEQ", "GTEQ", "SHL", "SHR", 
	"assign", "COMMA", "LOR","CALL", "CAST", 
	"LNOT", "NOT", "NEG","const","ident","ARRAY",
	"PTR", "ADDR",

	"istr:expr","DECLSTAT", "BODY", "istr:block", "istr:return", "istr:while", "istr:do",
	"istr:if", "istr:for", "",
	"", "",  "", "","",

};

// Funzioni di utilita'
namespace XMLGen {
	struct _tagTerm{
		const char * psz;
		int beg;
		_tagTerm(const char * n, int k = 1) : psz(n), beg(k) {}
		friend ostream & operator << (ostream & o, const _tagTerm &);
	};
	ostream & operator << (ostream & o, const _tagTerm &s)
	{ 
		if(!s.psz) 
			o << (s.beg ? "/>" : ">"); 
		else o << (s.beg ? "<" : "</") << s.psz << (s.beg ? "": ">"); 
		return o;}

	struct _tagAttrib {
		const char * _nome, *_valore;
		
		_tagAttrib(const char * n, const char* v) : 
			_nome(n), _valore(v) {}
		friend ostream & operator << (ostream & o, const _tagAttrib &);
	};
	ostream & operator << (ostream & o, const _tagAttrib &s)
	{ o << " " << s._nome << "=\"" << s._valore << "\""; return o;}

	_tagTerm tagBegin(const char * nome) { return _tagTerm(nome);}
	_tagTerm tagClose(bool ended = false)		 { return _tagTerm(0, ended ? 1 : 0);}	
	_tagTerm tagEnd(const char * nome)   { return _tagTerm(nome,0);}
	_tagAttrib tagAttrib(const char * nome, const char * valore)
	{return _tagAttrib(nome, valore);}
	_tagAttrib tagAttrib(const char * nome, int v)
	{
		static char buf[20];
		ostrstream os(buf, sizeof(buf));
		os << v << ends;	
		return _tagAttrib(nome, os.str());
	}
};

using namespace XMLGen;

void CNode::writeXml(ostream & o, writeXmlData&k)
{	
	o << tagBegin(tagNode[*this]) << tagClose();
	if(left()) 
		left()->writeXml(o, k);
	if(right())
		right()->writeXml(o, k);
	o << tagEnd(tagNode[*this]);
}


void CDirDecl::writeXml(ostream& o, writeXmlData&k)
{
	o << tagBegin(tagNode[*this]) ;
	if(identifer()) 
		o << tagAttrib("id", identifer());
	o << tagClose();
	if(left()) 
		left()->writeXml(o, k);
	if(right())
		right()->writeXml(o, k);
	o << tagEnd(tagNode[*this]);
}

void CComma::writeXml(ostream & o, writeXmlData&k)
{
	o << tagBegin(tagNode[*this]) << tagClose();
	iterator it = begin();
	while(it != end()) {
		assert(*it);
		(*it)->writeXml(o, k);
		it++;
		o << endl;
	}
	o << tagEnd(tagNode[*this]);
}

// espressione costante
// <CONST value="forma stringa" tipo="sig"/>
void CConstExpr::writeXml(ostream & o, writeXmlData&)
{	
	ostrstream os;
	o << tagBegin(tagNode[*this]) << " type = \"" << *ptipo << "\"";
	switch(ptipo->getType()) {
	case tINT: os << ival; break;
	case tCHAR: os << cval; break;
	case tFLOAT: os << fval; break;
	case tPTR: os << strval.cp; break;
	default:
		break;
	}
	os << ends;
	o << tagAttrib("value", os.str());
	o << tagClose(1);
	delete[] os.str();
}


void CIdentifer::writeXml(ostream & o, writeXmlData& )
{
	o << tagBegin(tagNode[*this]) <<
		tagAttrib("id", identifer()) << tagClose(1);	
}

void CStatement::writeXml(ostream & o, writeXmlData& k)
{

	o << tagBegin(tagNode[*this]) << 
		 tagAttrib("line", lineNumber()) <<
		tagClose();
	if(left()) 
		left()->writeXml(o, k);
	if(right())
		right()->writeXml(o, k);
	o << tagEnd(tagNode[*this]);

}


void CBlock::writeXml(ostream& o, writeXmlData& k)
{
	// top level no
	if(parent()) 
		o << tagBegin(tagNode[*this]) 
		<< tagAttrib("line", lineNumber()) << tagClose() << endl;
	else {
		o << tagBegin("cppsource") 
			<< tagAttrib("source", k.source)
			<< tagClose();
	}

	iterator it = begin();
	while(it != end()) {
		assert(*it);
		(*it)->writeXml(o, k);
		o << endl;
		it++;
	}
	if(parent()) 
		o << tagEnd(tagNode[*this]) << endl;
	else
		o << tagEnd("cppsource");
}


void CCondStat::writeXml(ostream& o, writeXmlData& k)
{
	o << tagBegin(tagNode[*this]) << 
		tagAttrib("line", lineNumber()) << tagClose() << endl;

	switch(node()) {
	case Nif:
		assert(bcond);
		assert(block);
		// bcond->writeXml(o,k);
		block->writeXml(o, k);
		if(blockII) blockII->writeXml(o, k);
		break;
	case Nwhile:
		assert(bcond && block);
		// bcond->writeXml(o, k);
		block->writeXml(o, k);
		break;
	default:
		return;
	}
	o << tagEnd(tagNode[*this]) << endl;

}

void CExpression::writeXml(ostream & o, writeXmlData& k)
{
	o << tagBegin(tagNode[*this]) << tagClose() << endl;
	
	if(left()) 
		left()->writeXml(o, k);
	if(right())
		right()->writeXml(o, k);
	o << tagEnd(tagNode[*this]);
}

