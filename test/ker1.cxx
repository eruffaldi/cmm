#include <stdio.h>


void main()
{
        int c, nl, nw, nc, state;
        int IN, OUT;
        IN = 1;
        OUT = 0;
        state = OUT;
        nl = nw = nc = 0;
        while((c = getchar()) != EOF) {
                nc = nc+1;
                if(c == '\n')
                        nl = nl+1;
                if(c == ' ' || c == '\n' || c == '\t')
                        state = OUT;
                else if( state == OUT)
                {
                        state = IN;
                        nw = nw +1;
                }
        }
        printf("%d %d %d\n", nl, nw, nc);
}
