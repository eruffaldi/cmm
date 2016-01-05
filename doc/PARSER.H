// Decl Parser: in input abbiamo una keyword valida per una dichiarazione, 
// in uscita deve essereci una CDecl dotata del proprio albero di parsing, 
// e opzionlemente di identificatore. I token ammessi in uscita sono: 
// "; , = { "

PDecl dcl ( PBaseDecl base) 
{
	p = nuovo CDecl
	ident = ""
	try {
		p->dcl = dirdcl(base);
	}
	catch {

	}
}

// eccezioni generate: 
///1) quelle di parseexpression
//	  recovery: possiamo scartare fino a ], l'importante e' liberare la memoria
// 2) quelle di processafunc
//	  deallocare la memoria di primo e di rec
PDeclarator dirdcl (PDeclarator base) 
{
	PDeclarator primo, rec;
	primo = rec = 0;

	while ('*') 
		base = new CPtrDecl(base);

	if '(' {
		rec = dirdcl(0);
	}

	if identifer {
		ident = identifer
	}

	if '[' 
		PDeclarator ultimo;
		ultimo = primo = 0;
		while '[' {
			PDeclarator sizeexpr = parseexpression
			PDeclarator temp = new CArrayDecl(0, sizeexpr)
			if primo == 0 {
				ultimo = primo = temp;
			}
			else {
				ultimo->first = temp;
				ultimo = temp;
			}
		}
		ultimo->first = base;
	else if '(' 
		// funzione 
		primo = funcdecl(base)
	else
		primo = base

	if rec {
		// scorri rec fino a trovare la fine e aggancia primo
		primo = rec
	}
	return primo
}	

PDeclarator funcdecl(PDeclarator basereturntype)
{
	PFuncDecl p = new CFuncDecl(basereturntype);
	PDecl arg;

	do {
		if "..." allora p->add(new CNode(Nellipse)); e break
		if ")" allora break

		if not (canBeDecl()) allora {
			distruggi p
			raise exception "unexpected token"
		}
		arg = dcl(new CBaseDecl(..));
		if "=" allora
			skip_to ","
			unsupported argument init error
		else 
			p->add(new CDeclStatement(arg));

	} while ","

	if non c'e' ")" {
		delete p
		raise Expected )
	}
	return p;
}



// DeclStatement
void declStatement(inFunction, CBlockStatement & block)
	CBaseDecl base(externflag, tipobase);
	
	do {
		PDecl p = dcl(new CBaseDecl(base));
		if (p->ident() == 0) {
			// expected name. delete p; skip to ";" 
			return;
		}
		else {
			if "{" {
				block.add(functionbody(p));	// CFuncBody
				return;
			}
			else if "=" {
				block.add(initvariable(p));	// CDeclStatement
			}
			else {
				block.add(new CDeclStatement(p));				// CDeclStatement senza init
			}
		}
	} while (",") ;
	if not ";" {
		expected SEMICOLON.
		// non c'e' recoveri, continua da qui
	}


PFuncBody functionbody(PDecl functiondecl) 
{
	PFuncBody p = new CFuncBody(functiondecl);
	setta flag in Function del parser
	try 
		p->setBlock(statement());
	catch {
		p->setBlock(0)
		return p;
	}
}

PStatement statement()
{
	PStatment p;
	if "{" {
		avanza();
		PBlockStatement p = new CBlockStatement();
		salva vecchio blocco
		setta p come il nuovo
		while( != "{" )
			try {
				p->add(statement());
			}
			catch {
				ripristina vecchio blocco e ritorna p
				break;
			}
		} 
		return p;
	}
	switch token {
		case if
		case while
		case for
		case do
		case break
		case "}": ritorna 0; senza fare altro						  
			try {
				DeclStatement(true, currentblock)			
			}
			return p;
		default:
			try {
				p = parseexprstatement
			}
			catch {
				skip fino a ;
				return 0;
			}
			required ";"
			return p;
	}
}



// Expressions
// ritornano un tipo PExpression
// generano eccezioni


PExpression element()
{
	if costante {
		return new CConstExpr(valore e tipo);
	}
	else if identificatore {
		return new CIdentifer(ident)
	}
	// else if "(" CASTING
	else if ( {
		try {
			p = topexpression
			if not ) allora 
				delete p
				errore
		}
		catch {
			return p;
		}
	}

	switch token {
	case tilde:	Nnot
	case plus: 0
	case minus: Nneg
	case ...
		PExpression p = primary();
		if p {
			return new CUnExpr(op, p);
		}
		else
			raise exception
	default:
		raise unexpected token
	}
}

PExpression primary()
{
	PExpression p = element();
	while(token != [ o ( allora esci) {
		switch token 
		case [:	  
			p2 = expression();
			attendere ]
			p = new CBinExpr(Nsubscribe, p, p2)

		case (
			p2 = commaexpression()
			attendere )
			p = new CCallExpr(p, p2)

	}
}

... le altre sono abbastanza noiose 
// Per semplificare le altre potremmo fare cose':

pexpr binaryexpr(STATO)
	p1 = primary
	se il token non e' in tabella ritorna p1
	altrimenti chiama p2 = binaryexpr(NUOVOSTATO)
	ritorna nodo(STATO, p1, p2)

stato
ASSIGN	=
LOR		||
LAND	&&
BOR		|
BXOR	^
BAND	&
RELEQ	== !=
RELCMP	< > <= >=
SHIFT	<< >>
ADD		+ -
MUL		* / % 
PRIMARY 

Partiamo da un certo stato s:
	applico PRIMARY
	parto dal basso e cerco quale stato corrisponde al token attuale,
	in tal caso mi sposto li'. Questo e' il nodo attuale. Ripeto ricorsivamente
	richiamando ancora PRIMARY. quando sono risalito ho il prossimo

	a + 4 * 2

		+
	a
		*

	4

	2	// non trova piu' niente

	  *
	4   2

		+ 
	a		*
		  4    2

	a + 4 + 3
		+ 
	 a