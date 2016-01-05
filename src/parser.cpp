#include <scan.h>
#include "parser.h"

//- PARSER CLASS
// La classe BaseParser implementa un parser C++ senza
// coinvolgere la symbol table. Distinguiamo fra uno
// stato GLOBAL ed uno FUNCTION.
// Nel primo stato possiamo accettare solo dichiarazioni
// di dati e funzioni, niente istruzioni, quando poi 
// incontriamo una definizione di funzione scendiamo
// di un livello ed operiamo nella fase function.
// Il nodo radice e' un multinode ovvero accetta piu' figli
// sottoforma di lista di nodi, nello stato GLOBAL accettiamo
// solamente VARDECL, FUNCDECL, ciascuno dei quali ammette
// un sotto nodo per specificare l'init expression oppure
// il body stesso della funzione. Questo viene fatto per
// poter visualizzare l'albero sintattico.
// Il body stesso della funzione e' basato su una sequenza
// di dichiarazioni VARDECL, ISTRUZIONI ed ESPRESSIONI.
//
// L'albero generato deve poter essere utile sia per
// la generazione del codice, quindi con la possibilita'
// di indicare degli attributi, sia per la visualizzazione
// dell'albero sintattico.
//
// Classe lista utilizzata.
// class lista<T>
// {
//		classe iterator {
//			costruttore privato su datum
//			datum * elemento.
//			operator ++
//			operator --
//			succ()
//			prec()
//			*operator
//		}
//		elimina(iterator)
//		costruttore copia 
//		accoda(iterator)
//		iterator primo()
//		iterator ultimo()		// fine non ultimo elemento
//		inserisci(T, iterator)		// inserisce prima di..
//	protected:
//		datum * primo;
//		datum * ultimo;
// }

Parser::Parser(CScan & s): scan(s)
{
}

void Parser::inizia()
{
	// inizia il processo di parsing dal punto piu' alto
	// l'unita' di traduzione. Possiamo identificare
	// l'inizio di una dichiarazione di tipo in base:
	// parole chiave: struct, class, static, extern typedef
	//  (NON SUPPORTATE)
	//  int, float
	// tipi dell'utente: struct-class names o typedef names 
	//  (NON SUPPORTATI)
	while(!scan.current().isEof()) {
		Token & tkn = scan.current();
		if(!isTypeDeclIntro(tkn)) {
			//-- ERRORE: richiesta typedecl.
		}
		Nodo * pn = parseTypeDecl(tkn);
		Nodo * what;
		// A questo punto saremmo a posto per la fase di
		// parsing ma ci interessa anche costruire i simboli
		// percio' trasformiamo il nodo di parsing in un tipo
		// Infine se era presente un nome, viene estratto
		// NB: non gestiamo i modificatori, altrimenti
		// dovremmo controllare anche quelli.
		Type * tipo;
		char *nome;
		typeDeclToType(pn, nome);
	
		// nel caso di una funzione dobbiamo vedere se e' 
		// un prototipo da inserire nella tabella oppure
		// un function body
		if(tipo->isFunction()) {
			bool isProto;
			isProto = scan.current().tokentype() == ';';
			// altrimenti attenditi una compound
		}	
		else  {
			// richiede un ; per terminare
			requireTerminaIstruzione();
		}
		aggiungiAllaSymbolTable(nome, tipo, "variabile");
		
		// Il nodo what puo' essere un FUNCTIONBODY oppure
		// un DECLVAR.
		radice->figli.accoda(what);
	}
}

Parser::insideFunctionBody()
{
	NodoMulti* body;
	Nodo * pn;
	// precondizione: in entrata deve esserci {
	while(!scan.current.isEof()) {
		Token & tkn = scan.current();

		// termina function body
		if(tkn.tokentype() == '{') {
			scan.next();
			break;
		}

		if(!isTypeDeclIntro(tkn)) {
			pn = processStatement(tkn);			
			// controlla errori
			body->figli.accoda(pn);
		}
		else {
			pn = parseTypeDecl(tkn);
			Type * tipo;
			char *nome;
			typeDeclToType(pn, nome);
		
			// funzione dentro funzione, deve essere 
			// obbligatoriamente un proto
			// un function body
			if(tipo->isFunction()) {
				bool isProto;
				isProto = scan.current().tokentype() == ';';
				if(!isProto) {
					// errore
				}
			}	
			else  				
				requireTerminaIstruzione();
			// questa volta e' la symbol table della funzione
			aggiungiAllaSymbolTable(nome, tipo, "variabile");
			
			// il nodo what e' solo un DECLVAR
			body->figli.accoda(what);
		}
	}

}

// precondizione:  c'e' una funzione corrente
//				   c'e' una tabella locale attiva (nesting)
//				   abbiamo un conteggio del livello di nesting
Nodo * processStatement(Token & tkn) 
{
	if(tkn == KEYW) {
		switch(tkn.getKeyword()) {
		case K_IF:
		case K_WHILE:
		case K_RETURN:
		case K_FOR:
		default:
		}
	}
	
	// nel caso di una keyword tipo sizeof o altra usata
	// per una espressione generiamo una statement espressione

}