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
                nc = nc+1;
                if(c == '\n')
                        nl = nl+1;
        }
        printf("%d %d %d\n", nl, nw, nc);
}
