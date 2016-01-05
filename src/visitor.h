// Nome: visitor
// Progetto: CPP/Tools
// Autore: Ruffaldi Emanuele
// Descrizione: due macro per realizzare il design pattern
// visitor
#ifndef VISITOR_H
#define VISITOR_H

#define VISITOR_OF(class) virtual void Visit##class (class*p)
#define VISITABLE_BY(me, class) virtual void accept(class& v) \
        { v.Visit##me(this); }

#endif
