#include <iostream.H>
extern "C" {
extern float ww;
extern int q(int j);
extern int * p;
extern int glo;
extern  int v(int, char);
}

 int v(int j, char k)
{
    j = k;
    k = 10;
    return 10;
}

void main() {
    int r = q(16);
//    ww = 2.0;    
//    cout << "WW should be 4.5 = " << ww << endl;;
//  cout << "Value of p should be " << &glo << " = " << p << endl;
    cout << "Return value should be 8 " << r << endl;    
}
