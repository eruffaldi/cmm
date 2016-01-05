void printf(char * , ...)

float glo[10];

char * example(int k, float f)
{
        glo[k] = f;
        printf("This is a phrase %d , %s",
                k * 10 + (int)(glo[2]*2) , "Hllo ");
        return "Hello World";
}
