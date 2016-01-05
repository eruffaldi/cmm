// Nome: preproc.h
// Progetto: CPP/Lexer
// Autore: Ruffaldi Emanuele
// Descrizione: preprocessore C++
#include "preproc.h"
#include <string.h>

CPreproc::CPreproc(const char * nome)
: m_stream(nome)
{
	m_fileno = 0;
	m_lineno = 0;
	m_stream.unsetf(ios::skipws);
	m_filename = new char[strlen(nome)+1];
	strcpy(m_filename, nome);
}

bool CPreproc::skipWhite()
{
	bool isSpace = false;
	char c1, c2,c;
	while(1) {
		if(eof() || lowpeek() == EOF) {
			if(handleEOF()) 
				continue;
			else
				return false;
		}

		while(!eof() && lowpeek() != EOF) {						
			c = lowpeek();
			if(c == '\n' || c == '\r') {
				m_lineno++;
				c = lowget();
				isSpace = true;
			}		
			// se c'e' un # allora scarta direttiva preproc			
			else if(c == '#') {	
				skipLine();
				isSpace = true;
			}
			else if(c == '/') {
				lowget();
				c2 = lowpeek();				
				if(c2 != '/' && c2 != '*') {
					lowback(c);
					return isSpace;						
				}
				else
					isSpace = true;
				// commento "//"
				if(c2 == '/') 						
					skipLine();		
				// commento multiline
				else if(c2 == '*') 
					skipComment();				
			}
			else if(c == ' ' || c == '\t') {
				lowget();
				isSpace = true;
			}
			// al primo carattere non spazio ritorna space
			// se c'erano stati degli spazi
			else 
				return isSpace;
			
		}
	}
	return false;
}

// skip line, si posiziona all'inizio della nuova linea
// genera un errore unexpected end of file se termina
bool CPreproc::skipLine()
{
	char c;
	while((c = lowget()) != EOF) {
		if(c == '\n'|| c == '\r') {
			m_lineno++;
			return true;
		}
	}
	return false;
}

// un commento multilinea deve rimanere dentro ad un 
// file 
void CPreproc::skipComment()
{
	char c;
	while((c = lowget()) != EOF) {
		if(c == '*') {
			char c2;
			c2 = lowpeek();
			if(c2 == '/') {
				lowget();
				return;
			}			
		}
		else if(c == '\n' || c == '\r')
			m_lineno++;		
	}	
	throw unterminatedComment();
}

// Ritorna:
//	spazio		dopo aver estratto tutti gli spazi visibili
int CPreproc::get()
{	
	if(skipWhite()) 
		return ' ';
	else
		return lowget();
}

