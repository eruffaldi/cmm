// stack.cpp: implementation of the stack class.
//
//////////////////////////////////////////////////////////////////////

#include "stack.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

stackVector::stackVector(const int n)
{
	if (n <= 0) n = 1;
	vec = new T[n];
	cnt = n;
	pos = 0;	// cresce verso il basso
}

stackVector::~stackVector()
{
	delete[] vec;
}


void stackVector::push(const T & v)
{
	if(pos != cnt) 
		vec[pos++] = v;
}

void stackVector::pop()
{
	if(pos != 0) pop--;
}

T & stackVector::top()
{
	return vec[pos-1];
}

bool stackVector::empty() { return pos != 0;}
bool stackVector::full() { return pos != cnt;}
int stackVector::count() {return pos;}