// Nome: types
// Progetto: CPP/Semantic
// Autore: Ruffaldi Emanuele
//
// Descrizione: classe per la gestione dei tipi di dato, la classe CType
// incapsula le informazioni sui tipi di dato: l'enumerato BaseTypes e'
// l'informazione contenuta in CType, che puo' essere terminale o non
// terminale. Nel caso non terminale, un puntatore rappresenta il livello
// successivo.
//
// type: terminal | ptr | array | func
// terminal: void int float void long long64 double longdouble char ellipse
// ptr: "ptr" type
// array: "array [size]" type
// function:  type ( type, ..., type)
//
// La classe stessa gestisce in una hash table le informazioni sui tipi,
// in modo tale da ridurre l'occupazione di memoria.

//- Includes
#include <assert.h>
#include <iostream.h>
#include "types.h"
#include "htable.h"

// questa classe serve a confrontare due puntatori PType in base al
// valore.
class confronto {
public:
inline int operator()(PType p1, PType p2)
{ return (!p1 && !p2) || *p1 == *p2;}
};

HashishTable<PType, PType, confronto> tabellatipi;

//- Local Functions

//- Implementatione

// calcolo del valore hash di un oggetto CType
// tale algoritmo puo' essere migliorato
int hashValue(const PType &p)
{
    int result = p->base;
    result ^= int(p->next);
    if(p->base == tFUNC) {
        result ^= int(p->proto.aParametri);
    }
    return result;
}

// costruttore, terminale o ptr
CType::CType(BaseType b, PType n)
{
    assert(b <= tELLIPSE || b == tPTR);
    base = b;
    next = n;
}

CType::CType(CStruct*sym)
{
    assert(sym);
    base = tSTRUCT;
    symbol = sym;
}

CType::CType(BaseType b, PType n, int asize)
{
    assert(b == tARRAY && n != 0);
    base = b;
    next = n;
    arraySize = asize;
}

CType::CType(BaseType b, PType n, PType * params, int nparams, CallMode mode)
{
    assert(b == tFUNC && params != 0 && nparams >= 0);
    base = b;
    next = n;
    proto.nParametri = nparams;    
	proto.aParametri = new PType[nparams];
	assert(proto.aParametri != 0);
    for(int i = 0; i < proto.nParametri ; i++)
        proto.aParametri[i] = params[i];
    proto.aMode = mode;
}

PType CType::ptr() const
{
    CType tmp(tPTR, PType(this));
    return installa(&tmp);
}

PType CType::deref() const
{
    assert(isPtr() || isArray());
    return next;
}                

PType CType::func(PType * params, int nparams, CallMode mode) const
{
    CType tmp(tFUNC, PType(this), params, nparams,  mode);
    return installa(&tmp);
}

PType CType::array(int n) const
{
    assert(base != tELLIPSE);
    CType tmp(tARRAY, PType(this), n);
    return installa(&tmp);
}

PType CType::arrayToPointer() const
{
    assert(isArray() && next != 0);
    return next->ptr();
}

CType::~CType()
{
    if(base == tFUNC)
        delete proto.aParametri;
}

// tipi predefiniti
PType CType::m_floattype, CType::m_inttype, CType::m_voidtype, CType::m_chartype, CType::m_elltype;

void CType::init()
{
    CType tmp(tCHAR,0);
    m_chartype = installa(&tmp);
    tmp.base = tINT;
    m_inttype = installa(&tmp);
    tmp.base = tVOID;
    m_voidtype = installa(&tmp);
    tmp.base = tFLOAT;
    m_floattype = installa(&tmp);
	tmp.base = tELLIPSE;
	m_elltype = installa(&tmp);
}

PType CType::installa(PType src)
{    
    if(tabellatipi.containsKey(src)) 
        return tabellatipi.get(src);
    else {
        PType nuovo = src->clone();
        tabellatipi.put(nuovo, nuovo);
        return nuovo;
    }
}

PType CType::clone() const
{
    if(base == tARRAY)
        return new CType(tARRAY, next, arraySize);
    else if(base == tFUNC)
        return new CType(tFUNC, next, proto.aParametri, proto.nParametri, proto.aMode);
    else 
        return new CType(base, next);
}

bool CType::operator== (const CType &t) const
{
    if(base != t.base) return false;
    if(base == tARRAY)
        return arraySize == t.arraySize && next == t.next;
    else if(base == tFUNC)
        {
            if(proto.nParametri != t.proto.nParametri ||
                proto.aMode != t.proto.aMode ||
                next != t.next) return false;
            for(int i = 0; i < proto.nParametri; i++) {
                if(proto.aParametri[i] != t.proto.aParametri[i])
                    return false;                        
            }
            return true;                    
        }
	else if(base == tSTRUCT)
		return symbol == t.symbol;
    else
        return next == t.next;
}

 istream & operator >> (istream & in, PType & r)
 {
        char c;in >> c;
        if(!in) return in;
        switch(c) {
            case 'i': r = CType::Int(); break;
            case 'f': r = CType::Float(); break;
            case 'v': r = CType::Void(); break;
            case 'c': r = CType::Char(); break;
            case '*':
            {
                PType p;
                in >> p;
                if(in) r = p->ptr();
                break;
            }
            case '[':
            {
                int n = 0;
                if(in.peek() != ']')
					in >> n;
				if(in.peek() != ']')
				{
					in.setf(ios::badbit);
					return in;
				}
                in.get(c);
                if(!in) return in;

                PType p;
                in >> p;
                if(in && p)
                    r = p->array(n);
				else
				{
					in.setf(ios::badbit);
					return in;
				}
				break;
            }
            case '(':
            {
                // Inseriamo qui i parametri
                int n = 0;
                PType buf[32];
                // scandiamo ( tipo , tipo ... )
                while(in.peek() != ')') {
                    in >> buf[n];
                    if(!in) return in;
                    n++;
					if(n > 32)
					{
						in.setf(ios::badbit);
						return in;
					}

                    if(in.peek() == ',')    // virgola opzionale
                        in.get(c);
                }
                in.get(c);
                PType p;
                in >> p;
                if(!in) return in;
                r = p->func(buf, n, csStdCall);
                return in;
            }
            default:

					in.setf(ios::badbit);
        }
        return in;
 }

 ostream & CType::fullNameType(ostream & o) const
 {
	 const CType & r = *this;
	 if(r.isArray()) {
        o << "[" << r.getArraySize() << "]";
		r.deref()->fullNameType(o);
	 }
     else if(r.isPtr()){
		 o << "*";
		 r.deref()->fullNameType(o);
	 }
     else if(r.isFunc()) {
		 
        o << "function (";
        int n = r.getParamCount();
        for(int i = 0; i < n; i++) {
            r.getParameter(i)->fullNameType(o);
            if(i != n-1)
                o << ",";
        }
        o << ") returning ";
		r.functionReturn()->fullNameType(o);
     }
     else
        switch(r.getType()) {
            case tINT: o << "int"; break;
            case tCHAR:o<< "char"; break;
            case tFLOAT: o<< "float"; break;
			case tVOID: o << "void"; break;
			default: o << "?"; break;
        }
    return o;
 }

 ostream & operator << (ostream & o, const CType & r)
 {
     if(r.isArray()) 
        o << "[" << r.getArraySize() << "]" << *(r.deref());
     else if(r.isPtr())
        o << "*" << *(r.deref());
     else if(r.isFunc()) {
        o << "(";
        int n = r.getParamCount();
        for(int i = 0; i < n; i++) {
            o << *(r.getParameter(i));
        }
        o << ")";
        o << *(r.functionReturn());
     }
     else
        switch(r.getType()) {
            case tINT: o << 'i'; break;
            case tCHAR:o<< 'c'; break;
            case tFLOAT: o<< 'f'; break;
			case tVOID: o << 'v'; break;
			default: o << "?"; break;
        }
    return o;
 }


 
PType CType::structured(CStruct*sym)
{
	CType tmp(sym);
	return installa(&tmp);
}
