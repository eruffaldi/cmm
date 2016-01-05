// Test della classe CType
#include <iostream.h>
#include "types.h"

void main()
{
    CType::init();
    PType cc = CType::Char();
    cout << "char: " << *cc << endl;
    PType pc = cc->ptr();
    cout << "char ptr: " << *pc << endl;
    PType aty = cc->array(20);
    cout << "array[20] of char: " << *aty << endl;
    PType pc2= aty->arrayToPointer();
    PType c2 = pc->deref();
    cout <<"char e' uguale a *(&char)? " << (cc == c2) << endl;
    cout <<"ptr char e' uguale ad array of char to pointer to char ? "<< (pc == pc2) << endl;
    cout << "Immettere un tipo da riconoscere come sig:";
    cin >> c2;
    if(!cin) cout << "BAD INPUT\n";
    else
    cout << "Questo e' quello immesso: " << *c2 << endl;
    cout << "Dimensione della tabella: " << tabellatipi.size() << endl;

    cout << "Items:\n";
    HashishTable<PType, PType, confronto>::Enumeration e = tabellatipi.values();
    while(e.hasMoreElements()) {
        cout << *e.getValue() << endl;
        e.nextElement();
    }
}
