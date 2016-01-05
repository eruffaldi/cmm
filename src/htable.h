// Nome: HashishTable
// Progetto: CPP/Table
// Autore: Ruffaldi Emanuele
// Descrizione: tabella hash template
// Parametrizzabile rispetto alla chiave al tipo contenuto,
// al metodo di confronto fra chiavi
// Uso:
//  declare a hashValue(const T&) function, where T is the key type
//  it should return an int
//
#ifndef HTABLE_H
#define HTABLE_H

#include "pair.h"

template <class Key, class T, class compare = same<Key> >
class HashishTable {
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

    class aiter;
    friend class aiter;
    class aiter {
        const HashishTable &h;
        PNode item;
        int idx;        
    public:
        aiter(const aiter & r) : h(r.h), idx(r.idx), item(r.item) {}
        aiter(const HashishTable & k, int ) : item(0), h(k), idx(0) {}
        aiter(const HashishTable &r) : h(r) {
            for(idx = 0; idx < r.mCapacity && !r.mTable[idx]; idx++);
            if(idx != r.mCapacity)
                item = r.mTable[idx];
            else
                item = 0;
        }
        aiter & operator++() {
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

        bool operator== (const aiter &it) { return item == it.item; }
        bool operator!= (const aiter &it) { return !(*this == it); }
    protected:
        PNode citem() {return item;}
        
    };

    friend class keyiterator;
    friend class valiterator;

    class keyiterator :public aiter{
    public:
        typedef Key result;
		keyiterator(const HashishTable<Key,T, compare> & ht) : aiter(ht) {}
        keyiterator(const HashishTable<Key,T,compare> & ht, int) : aiter(ht,0) {}                
        result & operator*() { return aiter::citem()->key;}
    };

    class valueiterator:public aiter{
    public:
        typedef T result;
		valueiterator(const HashishTable<Key,T, compare> & ht) : aiter(ht) {}
        valueiterator(const HashishTable<Key,T,compare> & ht, int) : aiter(ht,0) {}                
        result & operator*() { return aiter::citem()->value;}
    };

    
    typedef valueiterator iterator;

    iterator begin() {return iterator(*this);}
    iterator end() {return iterator(*this, 1);}
    keyiterator kbegin() {return keyiterator(*this);}
    keyiterator kend() {return keyiterator(*this, 1);}
    
};

// Implementazione
#include "htable_impl.h"

#endif

