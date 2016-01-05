#include <iostream.h>
#include "insieme.h"


typedef insieme<int> set;

void stampa(set & r)
{
	set::iterator it = r.begin();
	for(;it != r.end(); ++it)
		cout << *it << " ";
	cout << endl;
}

void main()
{
	set s1;
	set s2;
	s1.insert(10);
	s1.insert(20);
	s1.insert(40);
	s2.insert(20);
	s2.insert(10);
	s2.insert(50);
	cout << "set 1: "; stampa(s1); 
	cout << "set 2: "; stampa(s2); 
	cout << "1 + 2: "; stampa(s2 + s1); 
	cout << "1 * 2: "; stampa(s2 * s1); 
	cout << "2 - 1: "; stampa(s2 - s1); 
	cout << "1 - 2: "; stampa(s1 - s2); 
	cout << "1 <= 2:" << (s1 <= s2) << endl;
	cout << "2 <= 1:" << (s2 <= s1) << endl;
}