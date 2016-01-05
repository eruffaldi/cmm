#include <iostream.h>
#include "mappa.h"



void main()
{
	mappa<int, char *> mp;

	mp[20] = "Hello";
	mp[50] = "Ciccia";
	mp[123] = "Kaput";

	mappa<int,char*>::iterator it = mp.begin();
	while(it != mp.end()) {
		cout << it->second << endl;
		it++;
	}
	cout << endl;
	mp.erase(20);

	it = mp.begin();
	while(it != mp.end()) {
		cout << it->second << endl;
		it++;
	}

}