int func(int k)
{
int * p1, * p2;
void * pv, *pv2;
int j;
char * p3;

        p1 = p2 + 1;            // ottimizza la costante
        p2 = 2 + p1;            // inverte 
        p1 = p2 - 1;
        p1 = p2 + j;                    
        j = p2 - p1;
/*
        j = p2 + p1;            // errore: addizione di puntatori!
        pv = pv + 2;            // errore: uso di void *                
        j = p3 - p2;            // differenza di puntatori diversi
*/
        return j;
}
