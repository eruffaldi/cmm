// Nome: preproc.h
// Progetto: CPP/Lexer
// Autore: Ruffaldi Emanuele
// Descrizione: dichiarazione del preprocessore C++


#include <fstream.h>

// La funzione virtuale di estrazione di un carattere
// ritorna due caratteri particolari 
// EOL ed EOF
//
// la get ritorna i caratteri, quando trova uno spazio avanza
// considera come spazio anche // e /*
// contando gli spazi; se arriva alla fine del file chiama handleEOF
// per implementare gli include. Ritorna EOF se handleEOF ritorna false;

class preprocException {};

class CPreproc {
    char * m_filename;		// filename corrente
    int    m_lineno;		// linea corrente
    int    m_fileno;		// file corrente
	ifstream m_stream;		// streaming di input
	
	
public:
	class unterminatedComment : public preprocException {};

	CPreproc(const char * cp);
	int fileno() const {return m_fileno;}
	int lineno() const {return m_lineno;}
	virtual int get(); 
	virtual bool eof() const {return m_stream == 0;}
	virtual bool ready() const {return m_stream != 0;}
	virtual const char * filename(int fileno) {return m_filename;}

protected:
	virtual bool handleEOF() {return false;}	
	virtual int lowget() { return m_stream.get();}
	virtual int lowpeek() {return m_stream.peek();}
	virtual void lowback(int c) {m_stream.putback(c);}

	void skipComment();
	bool skipLine();
	bool skipWhite();
};
