#include "pool.h"
#include <iostream>

// Pool
template <class T>
void MemPool<T>::newChunk()
{
        m_pool.push_front(new char[ChunkSize]);
        m_free = ChunkSize;
        cout << "New Chunk\n";
}

template <class T>
void * MemPool<T>::require(int bytes)
{
        if(m_free < bytes) 
                newChunk();
        lista::iterator it = m_pool.begin();
        char * cp = *it + (ChunkSize-m_free);
        m_free -= bytes;
        cout << "Allocated " << bytes << "\n";
        return cp;

}

template <class T>
MemPool<T>::pointer MemPool<T>::create(const T& v)
{
        m_count++;
        return new(*this) T(v);
}

template <class T>
MemPool<T>::pointer MemPool<T>::create(int n)
{
        m_count += n;
        return new(*this) T[n];
}


template <class T>
int MemPool<T>::count() const
{
        return m_count;
}

void main()
{
        //Proviamo pool

        MemPool<char> mp;
        for(int i = 0; i <100 ;i++) {
                char * cp = mp.create((int)100);
                                        
        }
}
