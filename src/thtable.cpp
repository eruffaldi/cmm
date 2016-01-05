#include "htable.h"

#include <iostream.h>
template <class T, class OBJ>
class MemFun {
    typedef void (OBJ::* memfunptr)(const T&);
    OBJ & _o;
    memfunptr _mf;
public:
    MemFun(OBJ &o, memfunptr mf) : _o(o), _mf(mf) {}
    void operator()(const T &t) { (_o.*_mf)(t);}
};


template <class T, class ARG, class PRED>
class BindSecond {
    ARG & _a;
    PRED _pr;
public:
    BindSecond(ARG & a, PRED pr, T * dummy) : _a(a) _pr(pr) {}
    void operator()(const T &t) { _pr(t, a);}
};

template <class It, class Action>
void for_each(It f, It l, Action ac) {
    while(f != l) {
        ac(*f);
        ++f;
    }
}

class Hello {
public:
    void print(const int & i) {cout << i << endl;}
};

void main()
{
    HashishTable<int, int> ht;
    ht.put(10, 20);
    ht.put(12, 22);
    ht.put(1, 2);
    ht.put(1002,0020);
    Hello h;
    cout << "Values\n";
    for_each(ht.begin(), ht.end(), MemFun<int, Hello>(h, &Hello::print));
    cout << "Keys\n";
    for_each(ht.kbegin(), ht.kend(), MemFun<int, Hello>(h, &Hello::print));
}
