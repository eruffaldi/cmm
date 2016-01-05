// #include <stdio.h>
void printf(char * , ...);
int getchar();


void main()
{
        int c, nl, nw, nc, state;
        int IN, OUT,EOF;

        IN = 1;
        EOF = -1;
        OUT = 0;
        state = OUT;
        nl = nw = nc = 0;

        while((c = getchar()) != EOF) {
                ++nc;
                if(c == '\n')
                        ++nl;
                if(c == ' ' || c == '\n' || c == '\t')
                        state = OUT;
                else if( state == OUT)
                {
                        state = IN;
                        ++nw;
                }
        }
        printf("%d %d %d\n", nl, nw, nc);
}
