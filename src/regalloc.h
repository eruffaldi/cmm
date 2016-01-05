// Allocazione dei registri per AXP
#ifndef REGALLOC_H
#define REGALLOC_H

#include "htable.h"

// Questo algoritmo attraverso una discesa ricorsiva dei nodi
// dell'albero semantico calcola i registri richiesti. 
// * se e' una foglia 
//	 * se e' figlia sx allora regcount = 1;
//   * se e' dx allora regcount = 0;
// * binario t1, t2
//    t1 != t2 ==> max(t1, t2)
//	  t1 = t2 ==> t1 + 1
// * unario t
//    t != 0 ==> t
//    1
class RegAlloc {
protected:
	HashishTable<PNode, int> table;
public:
	RegAlloc();
	void ricorri(PNode, bool sx = false );
	// DEVE RICORRERE IN BASE AL TIPO DEL NODO 
	// INSERIRE NELLA TABELLA ED ESTRARRE LA COPPIA
	// NB. RICORRE SEMPRE
};

#endif