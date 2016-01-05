#include <iostream.h>
#include "NatSet.h"

void showSet(NatSet s) {
    unsigned int n;
    s.begin(n);
    while(s.ok(n)) cout << s.next(n) << " ";
    cout << endl;
}

/*
void main()
{
    NatSet r1, r2;
    int n;
    char c;
    cout << "Test NatSet. Immettere interi terminati da .\n";
    cout << "r1:";
    while(cin >> n)
        r1 += NatSet(n);
    cin.clear();
    cin >> c;
    if(c != '.') {
        return;
    }
    cout << "r2:";
    while(cin >> n)
        r2 += NatSet(n);
    cin.clear();
    cin >> c;
    if(c != '.') {
        return;
    }
    cout << "r1 = "; showSet(r1); 
    cout << "r2 = "; showSet(r2);
    cout << "r2 * r1 = "; showSet(r2 * r1);
    cout << "r2 + r1 = ";  showSet(r2 + r1);
    cout << "r2 - r1 = ";  showSet(r2 - r1);
    cout << "r1 - r2 = ";  showSet(r1 - r2);
    cout << "r2 >= r1 " << (r2 >= r1 ? "true" : "false") << endl;
    cout << "r2 <= r1 " << (r2 <= r1 ? "true" : "false") << endl;
    cout << "Immettere elemento da testare in r2:";
    cin >> n;
    cout << "r2(" << n << ") " << (r2.elem(n)? "" : "not") << "present\n";

}
                                                   
*/
