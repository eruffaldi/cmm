// stack.h: interface for the stack class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STACK_H__1F06EF23_495D_11D2_915E_083507C10000__INCLUDED_)
#define AFX_STACK_H__1F06EF23_495D_11D2_915E_083507C10000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// stack implementato come vettore
template<class T>
class stackVector<T>
{
	T * vec;
	int pos;
	int cnt;
public:
	stackVector(const int s);
	virtual ~stackVector();
	void push(const T & v);
	void pop();
	T & top();
	bool empty();
	bool full();
	int count();
};

#endif // !defined(AFX_STACK_H__1F06EF23_495D_11D2_915E_083507C10000__INCLUDED_)
