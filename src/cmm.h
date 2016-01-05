// Nome: cmm.h
// Descrizione: funzioni base compilatore
// Autore: Ruffaldi Emanuele
// Progetto: CPP/Base
#ifndef CMM_H
#define CMM_H

#include "errors.h"

class CFunction;

class CMM {
public:
	// associa ad un codice di errore una stringa di testo
	static const char * lookupError(ErrorCode);
	// stampa l'errore cp
	static void error(const char * cp);
	// imposta la linea ed il file correnti per la notifica degli errori
	static void setLineRef(int fileno, int lineno) {
		currentLine = lineno;
		currentFile = fileno;}
	// imposta la funzione corrente per la notifica degli errori
	static void setFunction(CFunction *f) {currentFunction = f;}
	// ritorna il numero di errori del programma
	static int getErrorCount() {return errorCount;}
protected:
	static int 	currentLine, currentFile;
	static CFunction*	currentFunction;
	static int			errorCount;
};

#endif