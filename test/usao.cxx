#include <stdio.h>
extern "C" {
	int q(int j, int,int,int,int,int,int);
	int q2(int j, int,int,int,int,int,int);

 extern int glo;
};

void main() {
	int r = q(100,0,1,2,3,4,5);
	printf("Returned %d, and glo is now %d \n", r, glo);
	r = q2(0,1,2,3,4,5,6);
	printf("Q2 returned %d\n", r);
}
