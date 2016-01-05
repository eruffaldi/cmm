// Test source file for declarations

int a[10], hello, *pippo;
float ww;
int * p;
int glo;



int q2(int j, int a1, int a2, int a3, int a4, int a5, int a6)
{
return j;
}

int q(int j, int a1, int a2, int a3, int a4, int a5, int a6)
{
char c;
 // ww = a['a'] + (2+3); // OK
	glo = j;
// ww = 2.5 + ww;   // OK
// p = &glo; // OK
//    return v(100, 'a');
//    return 10+j, (j >> 1)*2;
//	c = j == 2 || (j > 20 && glo < j);
//	c = j == 2;
//	while(c > 20) {
//		glo = 2;
//	}

	return j+a6 + q2(0,1,2,3,4,5,6);
}



