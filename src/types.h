// Nome: types
// Progetto: CPP/Semantic
// Autore: Ruffaldi Emanuele
// Descrizione: dichiarazione gestore dei tipi
#ifndef TYPES_H
#define TYPES_H

#include <assert.h>
//  Signature 
//  funzione: ( tipo tipo tipo tipo ) tipo
//  array: [ size ] tipo
//  ptr: * tipo
//  terminali: i c f d l v L s z u
//
//- Classi per la gestione dei tipi
enum BaseType { tVOID, tINT, tSHORT, tLONG, tFLOAT, tCHAR,
    tDOUBLE, tINT64, tLONGDOUBLE,tENUM, tELLIPSE,tARRAY,
        tPTR, tFUNC, tSTRUCT};
enum CallMode { csCdecl, csStdCall};
// La classe CType memorizza una parte di un data-type, in particolare
// distinguiamo tra tipi terminale (i tipi primitivi del linguaggio)
// e non terminali, ad esempio ptr type -> next.
//
// Costruzione degli oggetti Type: non possono essere costruiti
// direttamente, infatti gli oggetti di tipo Type stanno in una cache

#include "dvector.h"

class CType;
typedef CType * PType;

int hashValue(const CType* &);

class CStruct;
class CType 
{
public:

	// costruzione di struttura
		static PType structured(CStruct*);
		
	//- costruzione del tipo funzione, 
        PType func(PType *,int, CallMode mode) const;
	// estrae il tipo ritornato dalla funzione
        inline PType  functionReturn() const; 
        inline PType  getParameter(int i) const;
        inline int    getParamCount() const;
        inline CallMode getCallMode() const;

	//- costruzione di un array dal tipo corrente
        PType array(int nelementi) const;
        PType arrayToPointer() const;
        inline int  getArraySize() const;

	//- costruzione del tipo puntatore/oggetto puntato
        PType ptr() const;
        PType deref() const;

	//- Tipi predefiniti
        inline static PType Void();
		inline static PType Ellipse();
        inline static PType Int();
        inline static PType Float();

	//- Controlli di base
    inline bool isFunc() const;
    inline bool isPtr()  const ;
    inline bool isInt() const;
    inline bool isFloat() const;
	inline bool canbePtr() const;
    inline bool isArray() const;
    inline BaseType getType() const;
    static inline PType Char() ;
    inline bool isScalar() const {return isIntegral() || base == tFLOAT; }
	inline bool isIntegral() const {return base == tINT || base == tCHAR;}
	CStruct * getStruct() const {return symbol;}
	 //bool isStruct() const;
	 //bool isUnion() const;
	 //bool isChar() const;
	 //bool isEnum() const ;
     bool operator== (const CType&  ) const;

    friend int hashValue(const PType &p);
        static void init();
                                 
    friend std::ostream & operator << (std::ostream & o, const CType & r);
	std::ostream & fullNameType(std::ostream & o) const;
private:

	BaseType  base;		// tipo di base
    PType   next;           // collegato 
	union {
		int  arraySize;
		struct {
			int		nParametri;
            PType* aParametri; // array dei tipi dei parametri
            CallMode aMode;
		} proto;
		CStruct * symbol;	// per ENUM, STRUCT, UNION
	};
        ~CType();
		CType(CStruct *);
        CType(BaseType b, PType n);
        CType(BaseType b, PType n, int asize);
        CType(BaseType b, PType n, PType *,int ,CallMode);
        PType clone() const;        // crea una copia allocando da un pool
protected:
		
	// internamente viene utilizzata una funzione per stabilire se
	// un oggetto e' presente. In pratica calcola l'hashvalue
	// e ricerca all'interno della tabella confrontando con precisione
	// il tipo del dato
        static PType installa(PType cerca);
        //
        // Ad esempio ptr(), fatti gli opportuni controlli potrebbe scrivere:
        //    CType tmp(PTR, this);
        //    return installa(&tmp);
        // Dove la funzione di installazione controlla prima se e' presente
        // nella tabella hash, altrimenti costruisce l'oggetto ricordandosi
        // che i puntatori next e parametri di funzione sono gia' nella
        // cache.
        // OGNI puntatore PType che esce dai limiti della classe CType
        // DEVE essere dentro la cache.
        static PType m_floattype, m_inttype, m_voidtype, m_chartype, m_elltype;
};


// Inlines
    inline bool CType::isFunc() const {return base == tFUNC;}
    inline bool CType::isPtr()  const {return base == tPTR;}
    inline bool CType::isInt() const {return base == tINT;}
    inline bool CType::isFloat() const {return base == tFLOAT;}
    inline bool CType::isArray() const {return base == tARRAY;}
	inline bool CType::canbePtr() const {return base == tARRAY || base == tPTR;}
    inline BaseType CType::getType() const {return base;}

    inline PType CType::Void() {return m_voidtype;}
	inline PType CType::Ellipse() {return m_elltype;}
    inline PType CType::Int() {return m_inttype;}
    inline PType CType::Float() {return m_floattype;}
    inline PType CType::Char() {return m_chartype;}

inline int CType::getParamCount() const
{
    assert(isFunc());
    return proto.nParametri;
}

inline CallMode CType::getCallMode() const
{
    assert(isFunc());
    return proto.aMode;    
}

inline int CType::getArraySize() const
{
    assert(isArray());
    return arraySize;
}

inline PType  CType::functionReturn() const
{
    assert(isFunc());
    return next;
}

inline PType  CType::getParameter(int i) const
{
    assert(isFunc() && i >= 0 && i < proto.nParametri);
    return proto.aParametri[i];
}

#endif
