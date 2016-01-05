#include "node.h"

CSymbolTable& CFunction::getTable() {return PBlock(codice)->table;}