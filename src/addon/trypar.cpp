#include <iostream.h>
#include "parser.h"
#include "types.h"

void  main()
{
	CParser par("parsrc.cxx");	
	CType::init();
	PBlock pb = par.parse();
	cout << "Parsing completed\n";
	pb->write(cout, 0);
}