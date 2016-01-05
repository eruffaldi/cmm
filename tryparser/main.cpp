#include <iostream.h>
#include "..\scan.h"
#include "..\keywords.h"
#include "..\parser.h"

// Test del componente di parsing, analizziamo il codice 
// sorgente di un programma C++ visualizzandone l'albero
// sintattico
void main(int argc, char * argv[])
{
	cout << "Parser Tester\n";
	if(argc == 1) {
		cout << "Required source C++ file\n";
		return -1;
	}
	CScan scanner(argv[1], KeywordList, sizeof(KeywordList)/sizeof(KeywordList[0]));

	if(!scanner) {
		cout << "Scanner non inizializzato. Termino.\n";
		return -2;
	}

	// Costruisce il parser, lo esegue agganciandolo 
	// allo scanner, genera un albero sintattico 

	Parser par;
	par.inizia(scanner);
	par.stampa(cout);
}