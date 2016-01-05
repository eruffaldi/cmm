struct W {
        char b[100];

};

W f(W p)
{
        p.b[29] = 100;
        return p;
}
