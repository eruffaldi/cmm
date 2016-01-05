#include <list>


using namespace std;
template <class T>
class MemPool {
        int m_count;    // oggetti di tipo T nel chunk
        int m_free;
        const int ChunkSize;
        typedef char * chunk;
        typedef list<chunk> lista;
        lista m_pool;
        
        void newChunk();        // crea un nuovo chunk e m_lastfree = 0
public:
        void * require(int bytes);
        typedef T * pointer;
        pointer create(const T &);
        pointer create(int n);
        int count() const;
        MemPool() : ChunkSize(4096) { m_free = -1;}
};

template<class _Ty>
        inline void *operator new(size_t _N, MemPool<_Ty>& _Al)
        {return (_Al.require(_N)); }

