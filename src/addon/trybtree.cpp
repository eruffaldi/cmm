#include <iostream.h>
#include "btree.h"


typedef binTree<char*> albStringhe;


void struttura(albStringhe::iterator it, int k = 0)
{	
	for(int i = 0; i < k;i++)
		cout << ' ';
	cout << *it << endl;
	if(it.left() != albStringhe::iterator ())
		struttura(it.left(),k+1);
	if(it.right() != albStringhe::iterator ())
		struttura(it.right(),k+1);
	
}

void main()
{
	albStringhe alb;
	alb.insert("Hell");
	alb.insert("Fuck");
	alb.insert("Pick");
	alb.insert("Good");
	albStringhe::iterator it = alb.begin();
	while(it != alb.end()) {
		cout << *it << endl;
		it++;
	}
	struttura(alb.root());
	it = alb.find("Hell");
	if(it == alb.end()) {
		cout << "Elemento non trovato\n";
	}
	else {
		alb.erase(it++);
		if(it != alb.end())
			cout << "Dopo il cancellato deve esserci Pick: " << *it << endl;
	}
	struttura(alb.root());
	alb.clear();
	cout << "Ora deve essere vuoto\n";
	it = alb.begin();
	while(it != alb.end()) {
		cout << *it << endl;
		it++;
	}

	alb.insert("Good");
	cout << "reinserito duplicato? " << alb.insert("Good").second << endl;
}