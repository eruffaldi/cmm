int k;
void func();
void func(int J);               // function proto redef

float jj;
int * jj;                       // symbol redef
void w()
{
k = j;
k = j + 2;

int q;
float q;                        // redefinition of local symbol

}


void w()                        // function redefinition
{
k = 2;
k = j + 2;
}

