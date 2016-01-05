#include <iostream.h>
#include "parser.h"
#include "types.h"
#include "semantic.h"
#include "gencode.h"
#include "axpgen.h"
#ifndef _MSC_VER
#include <unistd.h>
#else
#include <process.h>
#endif


void showSymTable(CSymbolTable & tab);

int assembleAXP(char * source, char *obj)
{
#ifndef _MSC_VER
	char * ar[] = 
	{
	"as",
	source,
	"-o",
	obj,
	0
	};
	return execvp("as", ar);
#else
	return 0;
#endif
}


void showFunction(CFunction * pfunc)
{
	cout << pfunc->getNome() << " function " << *pfunc->getTipo() << endl;
	
	if(!pfunc->proto) {
		cout << "--locals " << pfunc->frameSize << " bytes\n";
		showSymTable(pfunc->getTable());
		cout << "--\n";
	}
}

void showVariable(CSymbol * psym)
{
	char * kind = psym->isStatic() ? "static" : 
	psym->isParam() ? "param" : psym->isExtern() ? "extern":
	"var";
	cout << psym->getNome() << " " << kind <<" " << *psym->getTipo() << endl;
}

void showSymTable(CSymbolTable & tab)
{
	// process symbol table 
	CSymbolTable::iterator it = tab.begin();
	while(it != tab.end()) {
		CSymbol * psym = *it;
		if(psym->isFunction()) 
			showFunction(PFunction(psym));		
		else 
			showVariable(psym);
		++it;
	}
}

// output the symbol table 
int  main(int argc, char*argv[])
{
	if(argc == 1) return 0;
	else if(argc == 3) {
		TypeCheck::setAXP();
	}
	CParser par(argv[1]);	
	CType::init();
	PBlock pb = par.parse();
	cout << "Parsing completed\n";
	// pb->write(cout, 0);
	Semantic sem;
	sem.analyze(pb);
	cout << "Global symbols\n";
	showSymTable(pb->table);	// only topmost symbols!
	pb->write(cout, 0);
	if(sem.getErrorCount() == 0) {
		cout << "Code Generation...\n";
		if(argc != 3) {
			CodeGen cg(argv[1], "out.asm", pb);		
			return 1;
		}
		else {
			{AXPGen cg(argv[1], "out.s", pb);
			}
			return assembleAXP("out.s", "out.o");
		}
	}
	cout << "Errors: " << sem.getErrorCount() << endl;
	return 0;

}
