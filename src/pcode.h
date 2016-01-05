#ifndef PCODE_H
#define PCODE_H

// Specifiche del codice intermedio:
// Usiamo degli opcodes che identificano gli elementi di
// un albero molto simile a quello sintattico.
// I tipi di dato di questa macchina sono presenti 
// a questo livello come offsets rispetto al codice 
// principale. Tali tipi sono: 
//  C	signed char
//  D   double
//  F   float
//  I   integer
//  P   pointer
//  U   unsigned
//  S   short 
//  L   long
enum PType {
	pCHAR = 0,
	pINT = 1,
	pSHORT = 2,
	pFLOAT = 3,
	pLONG = 4,
	pUNSIGNED = 5,
	pDOUBLE = 6
};

enum PCode {
	pCOSTANTE,		// memorizza il valore
	pASSEGNA,		// binario
	pADD,			//  ""
	pSUB,			//  ""
	pMUL,			//  ""
	pDIV,			//  ""
	pCVI2F,			// conversione di tipo
	pCVF2I,			// 
	pRET,			// ritorna il valore
	pADDRESS,		// indirizzo di una espressione
	pLABEL,		
	pARG,			// inserisce un argomento
	pCALL,			
	pCOMPOSE,		// il primo e' quello a sinistra
	pLOAD			// carica il valore dall'indirizzo,unary
};
#endif