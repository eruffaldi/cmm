// PROGRAMMA DI PROVA
#include "slista.h"
#include <iostream.h>

void show(slista<int>& li)
{
    slista<int>::iterator it = li.begin();
    while(it != li.end()) {
        cout << *it << endl;
		it++;
    }
}

void main()
{
slista<int> li;

    li.push_back(100);
    li.push_back(200);
    li.push_back(240);
    li.push_back(120);
	li.push_front(50);
	li.push_front(20);
	li.push_back(120);

	slista<int>::iterator it = li.begin();    
    cout << "Lista singolarmente linkata" << endl; show(li);
	cout << "Popped front\n";
	li.pop_front();
	cout << "Nuovo front e' " << li.front() << endl;
	cout << "Il back e'  " << li.back() << endl;
	cout << "E' vuota? " << li.empty() << endl;
	cout << "Contiene " << li.count() << " elementi" << endl;
	cout << "Contiene " << li.count(120) << " di 120" << endl;
	
	slista<int> c2;
	c2.splice(c2.begin(), li, li.begin()+4);
	cout << "Spliced first element of li into c2\n";
	show(li);
	cout << "c2 = "; show(c2);
	li.sort();	
	cout << "Ordiniamola...\n";
	show(li);

/*
    cout << "Immettere valore da inserire in testa:";
    cin >> v;
    li.push_front(v);
    cout << "Il valore " << v << " e' presente? " << (li.isPresent(v) ? "si": "no") << endl;
    cout << "tale valore c'e' " << li.count(v) << " volte\n";
    show(li);
*/
/*
    cout << "Immettere valori separati da , e terminati da. ";
    slista<int> l2 ;
    while(cin) {
        cin >> v;
        l2.push_back(v);
        char c;
        cin >> c;
        if(c == '.')
            break;
    }
    cout << "Unione delle liste prima della posizione (-1 alla fine):";
    cin >> v;
    if(v == -1) {
        it = li.end();
    }
    else {
        it = li.begin();
        for(v--; v; it++, v--)
            ;
    }
    li.insert(it, l2.begin(), l2.end());
    show(li);
*/
}

