#include<unistd.h>

void main()
{
char * ar[] = 
	{
	"as",
	"-o",
	"out.o",
	"out.s",
	0
	};
	execvp("as", ar);
}; 
