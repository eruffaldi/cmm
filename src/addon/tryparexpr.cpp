#include <iostream.h>
#include "parser.h"
#include "types.h"

void  main()
{
	CParser par("exprsrc.cxx");
	par.next();	// accensione!
	CType::init();
	do {
		bool isDecl;
		PExpression p = 0;
		if(par.canBeDeclaration(par.current()) )  {
			p = par.parseDeclaration();
			isDecl = true;
		}
		else if(par.canBeExpression(par.current())) {
			p = par.parseExpression();
			isDecl = false;
		}
		else if(par.current() == ';') {
			par.next();
			continue;
		}
		else {
			cout << "What can it be? " << int(par.current()) << endl;
			par.next();
			continue;
		}

		if(!p) {
			cout << "Error in parsing\n";
			continue;
		}
		
		if(par.current() != ';')
			cout << "Richiedo ; \n";
		else
			par.next();

		cout << (!isDecl ? "Expression" : "declaration") << " found:\n";
		p->write(cout);
	} while(par.current() != tknFINEFILE);
}