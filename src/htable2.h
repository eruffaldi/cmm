#ifndef HTABLE_H
#define HTABLE_H

#include "pair.h"
// USAGE:
//  declare a hashValue(const T&) function, where T is the key type
//  it should return an int
template <class Key, class T, class compare = same<Key> >
class HashishTable {
    class Enumeration;
    class iterator;
    friend class HashishTable<Key,T,compare>::Enumeration;
    class TNode {
    public:
        Key key;
        T value;
        TNode* next;
        TNode(const Key&k, const T& v, TNode*n): key(k), value(v),
            next(n) {}
        TNode(const TNode& r) {
            key = r.key;
            value = r.value;
            if(r.next != 0)
                next = new Node(*r.next);
            else
                next = 0;
        }
        ~TNode() {if(next) delete next;}
    };

    
    typedef TNode Node;
    typedef Node* PNode;
    PNode* mTable;
    int mSize;
    int mCapacity;
    float mLoadFactor;
    int mExpandCapacity;
    void copia(const HashishTable<Key,T,compare>&);
    void rehash();
public:
    HashishTable(int initialCapacity = 107, float loadFactor = 1.0);
    HashishTable(const HashishTable&);
    HashishTable& operator= (const HashishTable&);
    void put(const Key&, const T&);
    T& get(const Key&k);
    const T& get(const Key& k) const;
    void clear();
    int size() {return mSize;}
    bool isEmpty() {return mSize == 0;}
    void remove(const Key&k);
    bool contains(const T&v);
    bool containsKey(const Key&k);
    typedef int KeyNotFound;

    friend class iterator;
    class iterator {
        const HashishTable &h;
        PNode item;
        int idx;        
    public:
        iterator(const iterator & r) : h(r.h), idx(r.idx), item(r.item) {}
        iterator(const HashishTable & k, int ) : item(0), h(k), idx(0) {}
        iterator(const HashishTable &r) : h(r) {
            // cerca il primo valore
            // assert(r.mTable != 0);
            for(idx = 0; idx < r.mCapacity && !r.mTable[idx]; idx++);
            if(idx != r.mCapacity)
                item = r.mTable[idx];
            else
                item = 0;
        }
        iterator & operator++() {
            if(item != 0) {
                if(item->next != 0)
                    item = item->next;
                else {
                    idx++;
                    for(; idx < h.mCapacity && !h.mTable[idx]; idx++);
                    if(idx < h.mCapacity)
                        item = h.mTable[idx];
                    else
                        item = 0;
                }
            }
            return *this;
        }

        Key & getKey() {return item->key;}
        T & operator *() {return item->value;}
        // T* operator ->() { return &(*this);}
        bool operator== (const iterator &it) { return item == it.item; }
        bool operator!= (const iterator &it) { return !(*this == it); }            
    };

    iterator begin() {return iterator(*this);}
    iterator end() {return iterator(*this, 1);}


    class Enumeration {
        const HashishTable &h;
        PNode item;
        int idx;
    public:
        Enumeration(const HashishTable & r) :  h(r) {
            // cerca il primo valore
//            assert(r.mTable != 0);
            for(idx = 0; idx < r.mCapacity && !r.mTable[idx]; idx++);
            if(idx != r.mCapacity)
                item = r.mTable[idx];
            else
                item = 0;
        }
        bool hasMoreElements() const {
            return !item == 0;               
        }
        const Key& getKey() const {return item->key;}
        const T& getValue() const {return item->value;}
        int getIndex() const { return idx;}
        void nextElement() {
            if(item->next != 0)
                item = item->next;
            else {
                idx++;
                for(; idx < h.mCapacity && !h.mTable[idx]; idx++);
                if(idx < h.mCapacity)
                    item = h.mTable[idx];
                else
                    item = 0;
            }
        }
    };
    Enumeration values() {return Enumeration(*this);};

};

// hashing functions
inline int hashValue(const char &c) {return int(c);}
inline int hashValue(const int &i) {return int(i);}

// Hashing di una stringa, sommo i caratteri e li moltiplico per 37
inline int hashValue(const char* const &p)
{
    int h = 0;
    for(int i = 0; p[i] != 0; i++)
        h = h*37 + p[i];
    return h;
}

inline int hashValue(const double &d)
{
    long bits = *((long *)&d);
    return (int)(bits ^ (bits >> 32));
}

inline int hashValue(const float &f)
{
    return *(int*)&f;
}


template<class Key, class T, class compare>
void HashishTable<Key, T,compare>::rehash()
{
    // ricalcola una nuova hash table
    int old = mCapacity;
    mCapacity = old*2 + 1;
    mExpandCapacity = (int) (mCapacity*mLoadFactor);
    PNode* oldTable = mTable;
    mTable = new PNode[mCapacity];
    for(int i = 0; i < mCapacity; i++)
        mTable[i] = 0;
    for(i = 0; i < old; i++) {
        PNode p = oldTable[i];
        while(p) {
            int hash = hashValue(p->key);
            int h = (hash & 0x7FFFFFFF) % mCapacity;
            PNode  pnext = p->next;
            p->next = mTable[h];
            mTable[h] = p;
            p = pnext;
        }        
    }
    delete[] oldTable;
//    cout << "Reashed from " << old << " to " << mCapacity << endl;
}

template<class Key, class T, class compare>
HashishTable<Key, T,compare>::HashishTable(int initialCapacity, float loadFactor)
{
    mCapacity = initialCapacity;
    mLoadFactor = loadFactor;
    mExpandCapacity = (int) loadFactor*mCapacity;
    mSize = 0;
    mTable = new PNode[mCapacity];
    for(int i = 0; i < mCapacity; i++)
        mTable[i] = 0;
}

template<class Key, class T, class compare>
HashishTable<Key,T,compare>::HashishTable(const HashishTable&r)
{
    copia(r);    
}

template<class Key, class T, class compare>
HashishTable<Key,T,compare>& HashishTable<Key,T,compare>::operator=(const HashishTable&r)
{
    if(&r != *this) {
        clear();
        delete[] mTable;
        copia(r);
    }
    return *this;
}

template<class Key, class T, class compare>
void HashishTable<Key,T,compare>::copia(const HashishTable&r)
{
    mCapacity = r.mCapacity;
    mTable = new PNode[mCapacity];
    mSize = r.mSize;
    mLoadFactor = r.mLoadFactor;
    for(int i = 0; i < mCapacity; i++) {
        if(r.mTable[i] != 0)
            mTable[i] = new Node(*r.mTable[i]);
        else
            mTable[i] = 0;
    }
}

template<class Key, class T, class compare>
void HashishTable<Key, T,compare>::put(const Key&k, const T& v)
{
    int hash = hashValue(k);
    int h = (hash & 0x7FFFFFFF) % mCapacity;
    PNode p = mTable[h];
    if(p == 0) {
        mSize++;
        mTable[h] = new Node(k, v, 0);
    }
    else {
        compare cmp;
        while(p) {
            if(cmp(p->key, k)) {
                p->value = v;
                return;
            }
            p = p->next;
        }
        mTable[h]->next = new Node(k, v, mTable[h]->next);
        mSize++;
    }
    if(mSize >= mExpandCapacity)
        rehash();
}

template<class Key, class T, class compare>
T& HashishTable<Key, T,compare>::get(const Key& k)
{
    int hash = hashValue(k);
    int h = (hash & 0x7FFFFFFF) % mCapacity;
    PNode p = mTable[h];
    compare cmp;
    for(; p; p = p->next) 
        if(cmp(p->key,k))
            return p->value;
    throw KeyNotFound();
    return p->value;
}

template<class Key, class T, class compare>
const T& HashishTable<Key, T,compare>::get(const Key& k) const
{
    int hash = hashValue(k);
    int h = (hash & 0x7FFFFFFF) % mCapacity;
    PNode p = mTable[h];
    for(; p; p = p->next) 
        if(compar(p->key,k))
            return p->value;
    throw KeyNotFound();
}                


template<class Key, class T, class compare>
bool HashishTable<Key, T,compare>::contains(const T& v)
{
    for(int i = 0; i < mCapacity; i++) {
        PNode p = mTable[i];
        while(p) {
            if(p->value == v)
                return true;
            p = p->next;
        }
    }
    return false;
}

template<class Key, class T, class compare>
bool HashishTable<Key, T,compare>::containsKey(const Key&k)
{
    int hash = hashValue(k);
    int h = (hash & 0x7FFFFFFF) % mCapacity;
    PNode p = mTable[h];
    compare cmp;
    while(p) { 
        if(cmp(k,p->key))
            return true;
        p = p->next;
    }
    return false;
}

template <class Key, class T, class compare>
void HashishTable<Key, T,compare>::remove(const Key&k)
{
    int hash = hashValue(k);
    int h = (hash & 0x7FFFFFFF) % mCapacity;
    PNode p = mTable[h];
    PNode prev = 0;
    compare cmp;
    while(p) {
        if(cmp(k,p->key)) {
            if(last == 0) 
                mTable[h] = p->next;
            else
                prev->next = p->next;
            p->next = 0;    // sgancia dalla catena
            delete p;
        }             
        prev = p;
        p = p->next;
    }

}

template <class Key, class T, class compare>
void HashishTable<Key, T,compare>::clear()
{
    for(int i = 0; i < mCapacity; i++) {
        if(mTable[i] != 0) {
            delete mTable[i];
            mTable[i] = 0;
        }
    }
    mSize = 0;
}


#endif

