#include <iostream.h>
#include "parser.h"
#include "types.h"

void  main()
{
	CParser par("declsrc.cxx");
	par.next();	// accensione!
	CType::init();
	do {
		PExpression p = par.parseDeclaration();
		// ready to spit out the p tree.
		if(par.current() != ';')
			cout << "Richiedo ; \n";
		else
			par.next();

		cout << "Declaration found:\n";
		p->write(cout);
	} while(par.current() != tknFINEFILE);
}