// Nome: cmm.cpp
// Descrizione: implementazione funzioni base compilatore
// Autore: Ruffaldi Emanuele
// Progetto: CPP/Base
#include "cmm.h"
#include <iostream>

const char * lookup[] = 
	{
	// first fatal
	"","",
	// first error
	"expected constant expression",
	"'void' type illegal always",
	"binary '%s': cannot convert '%s' to '%s'",
	"binary '%s': cannot convert '%s' to '%s'\nType deferred are different",
	"binary '%s': lvalue expression is required",
	"pointer expression required",
	"void function returning value",
	"array [] requires an array or pointer type",
	"array [] requires an integral index",
	"cannot use a void pointer as an array type (size unknown)",
	"pointer operation not implemented",
	"operator overloading not implemented",
	"binary '%s': integral required",
	"operator '%s': scalar type required",
	"cannot do typecast",
	"call requires function type operand",
	"too much arguments in call",
	"too less arguments in call",
	"cannot match argument %d from type %s to type %s",
	"function '%s': does not return a value",
	"function '%s': not all control paths return a value",
	"cannot add two pointers",
	"you can subtract two pointers of the same type",
	"array element, type cannot be function", 
	"symbol '%s': not found",
	"symbol '%s': function prototype different redefinition",
	"symbol '%s': redefinition",
	"function '%s': redefinition",
	"cannot cast pointer of differente base type, use a forced cast",
	"cannot cast to array or function types",
	// Parser errors
	"required token %c",
	"expected declaration in translation unit",
	"function body without a function", 
	"expected ; after declaration", 
	"arguments after ellipse argument not allowed",
	};

int CMM::currentLine;
int CMM::currentFile;
int CMM::errorCount;

const char * CMM::lookupError(ErrorCode ec)
{
	return lookup[ec];
}

void CMM::error(const char * cp)
{
	std::cout << "line " << currentLine << " - " << cp << std::endl;	
	errorCount++;
}
