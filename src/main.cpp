// Progetto: Compilatore C+- di Ruffaldi Emanuele
// Modulo principale
//
// Questo modulo contiene la funzione main che analizza i parametri
// immessi dall'utente ed attiva la compilazione in base alle 
// opzioni selezionate. Nel caso in cui si richieda l'effettiva
// compilazione viene chiamato l'assemblatore che deve essere 
// nel PATH. Per la compilazione con target Alpha l'assemblatore
// e' as (Digital UNIX), mentre per il target x86 l'assemblatore e'
// il MASM (Microsoft Macro Assembler 6.0+).
//
// La macro _MSC_VER indica se il compilatore e' costruito su di
// una macchina x86, in tal caso per default generiamo per x86 e
// richiamiamo l'assemblatore MASM; altrimenti generiamo per AXP e
// richiamiami AS. L'assemblatore non viene richiamato nel 
// cross-compiling.
//
// Opzioni di compilazione:
// S		genera solo il codice assembler senza richiamare l'assemblatore
//			il file viene messo in .s e .asm a seconda  
// GA		genera codice per AXP [default su AXP]
// G3		genera codice per 386 [default su x86]
// h		mostra l'help
// o file	nome del file per l'output, default .o e .obj 
// Fp		parser
// Ft		tabella simboli
// Fx		parser output in formato XML
// Fs		semantic
// P		stampa il testo di preprocessing su un file .i
// T		stampa il testo estratto dalla tokenizzazione su .tkn
#include <stdlib.h>
#include <iostream>
#include "parser.h"
#include "types.h"
#include "semantic.h"
#include "gencode.h"
#include "axpgen.h"
#ifndef _MSC_VER
#include <unistd.h>
#define AXPDEFAULTTARGET false
#else
#include <process.h>
#define AXPDEFAULTTARGET true
#endif

void info()
{
        std::cout << "Compilatore C+- 1.0\n"
				"by Ruffaldi Emanuele 1998\n"
                "Sintassi: cmm [opzioni] filename\n"
		"Fp\t\tstampa albero di parsing\n"
		"Fs\t\tstampa fase semantica\n"
		"Fx\t\tstampa albero di parsing come documento xml\n"
		"Ft\t\tstampa tabella simboli\n"
		"GA\t\tgenerazione per Alpha\n"
		"G3\t\tgenerazione per x86\n"
		"h\t\tmostra questo help\n"
		"P\t\tstampa il testo preprocessato\n"		
		"S\t\tgenerazione di codice assembler\n"
		"T\t\tstampa il tokenizzato\n";
        exit(-1);
}

const char * changeExtension(const char * nome, char * ext)
{
static char buf[255];
	const char * cp;
	cp = strrchr(nome, '\\') ;
	if(!cp) cp = strrchr(nome, '/');
	if(!cp) cp = nome;
	char * dotpos;
	dotpos = strrchr(cp, '.');
	memset(buf, 0, sizeof(buf));
	if(dotpos == 0) {
		strcpy(buf, nome);
		
	}
	else {
		strncpy(buf, nome, dotpos-nome);
		
	}
		strcat(buf, ".");
		strcat(buf, ext);
	return buf;
}

void showSymTable(CSymbolTable & tab);

int assembleAXP(char * source, char *obj)
{
#ifndef _MSC_VER
	const char * ar[] = 
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
	std::cout << pfunc->getNome() << " function " << *pfunc->getTipo() << std::endl;
	
	if(!pfunc->proto) {
		std::cout << "--locals " << pfunc->frameSize << " bytes\n";
		showSymTable(pfunc->getTable());
		std::cout << "--\n";
	}
}

void showVariable(CSymbol * psym)
{
	char * kind = psym->isStatic() ? "static" : 
	psym->isParam() ? "param" : psym->isExtern() ? "extern":
	"var";
	std::cout << psym->getNome() << " " << kind <<" " << *psym->getTipo() << std::endl;
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



// stampa solamente l'output del preprocessore
void stampaPreprocessed(char * source, char *output)
{
	CPreproc pre(source);
	ofstream of(output);
	char c;
	while(!pre.eof()) {
		c = pre.get();
		if(c == ' ')
			of << '\n';
		else
			of << c;
	}
}

// stampa i token estratti e reinseriti dentro lo stream
void stampaTokenized(char * source, char *output)
{
	ofstream of(output);
	CScanOnly scanner(source);
	int lineno = 0;
	while(scanner) {
		const Token & r = scanner.next();
		if(r.isEof())
			break;
		if(lineno != scanner.lineno()) {
			of << std::endl;
			lineno = scanner.lineno();
		}
		of << r << ' ';		
	}
}

int main(int argc, char * argv[])
{
char * source = 0;
char output[200] = "";
bool bAXPTarget = AXPDEFAULTTARGET;
bool bAssembleOnly = false;
bool bPrintParseTree = false;
bool bPrintSemanticTree = false;
bool bPrintSymbols = false;
bool bMakePreprocessOnly = false;
bool bMakeTokenized = false;
bool bPrintXML = false;

    if(argc == 1) info();
    for(int i = 1; i < argc; i++) {
		char * cp = argv[i];
		if(*cp == '-') {
			cp++;
			while(*cp != 0) {
				switch(*cp++) {
					case 'h': info(); break;
					case 'P': bMakePreprocessOnly = true; break;
					case 'T': bMakeTokenized = true; break;
					case 'G': 						
						switch(*cp++) {
						case 'A': bAXPTarget = true; break;
						case '3': bAXPTarget = false; break;
						default:
							std::cout << "Opzione G" << *(cp-1) << " non riconosciuta\n";
							break;
						}
						break;
					case 'F':
						switch(*cp++) {
						case 'p': bPrintParseTree = true; break;
						case 's': bPrintSemanticTree = true; break;
						case 't': bPrintSymbols = true; break;
						case 'x': bPrintXML = true; break;
						default:
							std::cout << "Opzione F" << *(cp-1) << " non riconosciuta\n";
							break;
						}
						break;
					case 'o':
						if(i+1 == argc) {
							std::cout << "Required output filename\n";
							exit(-1);
						}
						strcpy(output, argv[++i]);
						break;
					case 'S':
						bAssembleOnly = true;
						break;
					default: 
						std::cout << "Opzione " << *(cp-1) << " non riconosciuta\n"; 
						break;
				}
			}
		}
        else if(source != 0) {
			std::cout << "File sorgente gia' specificato\n";
            exit(-1);
        }
        else 
            source = argv[i];       
    }

    if(source == 0) {
            std::cout << "Il file sorgente non e' stato specificato!\n";
            exit(-1);
    }
	else {
		std::ifstream ifs(source);
		if(!ifs) {
			std::cout << "Il file sorgente " << source << std::endl;
			exit(-1);
		}
	}

	if(output[0] == 0) 
		strcpy(output, changeExtension(source, bAXPTarget ? "o":"obj"));
	

	PBlock proot;
	int ecount;
	// Init
	CType::init();
	
	if(bMakePreprocessOnly) {
		stampaPreprocessed(source, changeExtension(source, "i"));
		return 0;
	}
	else if(bMakeTokenized) {
		stampaTokenized(source, changeExtension(source, "tkn"));
		return 0;
	}

	// Entering Parsing
	{
		CParser par(source);	
		proot = par.parse();
	}
	// std::cout << "Parsing completed\n";
	if(bPrintParseTree && proot) {
//		proot->write(std::cout,writeData());
		PrintVisitor pv(std::cout, proot);
	}

	if(bPrintXML && proot) {
		std::ofstream of(changeExtension(source, "xml"));
		proot->writeXml(of, writeXmlData(source));
	}

	
	// Entering Semantic Analisys
	{
		Semantic sem;
		sem.analyze(proot);
		ecount = CMM::getErrorCount();
	}
	if(bPrintSemanticTree) 
		{PrintVisitor pv(std::cout, proot);}

	if(bPrintSymbols) {
		std::cout << "Global symbols\n";
		showSymTable(proot->getSymTab());	
	}

	// Starting code generation
	if(ecount == 0) {
		std::cout << "Code Generation...\n";
		char * asmTarget = changeExtension(source, 
			bAXPTarget ? "s" : "asm");
		if(!bAXPTarget) {
			CodeGen cg(source, asmTarget, proot);		
			return 1;
		}
		else {
			AXPGen cg(source, asmTarget, proot);
			return assembleAXP(asmTarget, output);
		}
	}	

	if(ecount)
	std::cout << "\n\t" << ecount << " error(s)" << std::endl;
	return 0;

}
