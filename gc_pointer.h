#ifndef GC_POINTER_H
#define GC_POINTER_H

#include <iostream>
#include <list>
#include <typeinfo>
#include <cstdlib>
#include "gc_details.h"
#include "gc_iterator.h"

template <class T, int size = 0>
class Pointer
{
private:
    // refContainer maintains the garbage collection list.
    static std::list<PtrDetails<T>> refContainer;
    static bool first; // true when first Pointer is created
    static int out_scope_num; // the number of Pointers go out of scope
    // addr points to the allocated memory to which
    // this Pointer pointer currently points.
    T *addr;
    /*  isArray is true if this Pointer points
        to an allocated array. It is false
        otherwise.
    */
    bool isArray;
    // true if pointing to array
    // If this Pointer is pointing to an allocated
    // array, then arraySize contains its size.
    unsigned arraySize; // size of the array
    // Return an iterator to pointer details in refContainer.
    typename std::list<PtrDetails<T>>::iterator findPtrInfo(T *ptr);

public:
    // Define an iterator type for Pointer<T>.
    typedef Iter<T> GCiterator;
    // Empty constructor
    // NOTE: templates aren't able to have prototypes with default arguments
    // this is why constructor is designed like this:
    Pointer()
    {
        Pointer(NULL);
    }
    Pointer(T *);
    // Copy constructor.
    Pointer(const Pointer &);
    // Destructor for Pointer.
    ~Pointer();
    // Collect garbage. Returns true if at least
    // one object was freed.
    static bool collect();
    // Overload assignment of pointer to Pointer.
    T *operator=(T *t);
    // Overload assignment of Pointer to Pointer.
    Pointer &operator=(Pointer &rv);
    // Return a reference to the object pointed
    // to by this Pointer.
    T &operator*()
    {
        return *addr;
    }
    // Return the address being pointed to.
    T *operator->() { return addr; }
    // Return a reference to the object at the
    // index specified by i.
    T &operator[](int i) { return addr[i]; }
    // Conversion function to T *.
    operator T *() { return addr; }
    // Return an Iter to the start of the allocated memory.
    Iter<T> begin()
    {
        int _size;
        if (isArray)
            _size = arraySize;
        else
            _size = 1;
        return Iter<T>(addr, addr, addr + _size);
    }
    // Return an Iter to one past the end of an allocated array.
    Iter<T> end()
    {
        int _size;
        if (isArray)
            _size = arraySize;
        else
            _size = 1;
        return Iter<T>(addr + _size, addr, addr + _size);
    }
    // Return the size of refContainer for this type of Pointer.
    static int refContainerSize() { return refContainer.size(); }
    // A utility function that displays refContainer.
    static void showlist();
    // Clear refContainer when program exits.
    static void shutdown();
};

// STATIC INITIALIZATION
// Creates storage for the static variables
template <class T, int size>
std::list<PtrDetails<T>> Pointer<T, size>::refContainer;
template <class T, int size>
bool Pointer<T, size>::first = true;
template <class T, int size>
int Pointer<T, size>::out_scope_num = 0;

// Constructor for both initialized and uninitialized objects. -> see class interface
template <class T, int size>
Pointer<T, size>::Pointer(T *t) : arraySize(size)
{
    // Register shutdown() as an exit function.
    if (first)
        atexit(shutdown);
    first = false;

    PtrDetails<T> ptr(t, size);
    ptr.refcount++;
    addr = t;

    refContainer.push_back(ptr);

    if (size > 0)
        isArray = true;
}
// Copy constructor.
template <class T, int size>
Pointer<T, size>::Pointer(const Pointer &ob)
{
    typename std::list<PtrDetails<T>>::iterator p;
    p = findPtrInfo(ob.addr);
    p->refcount++;
    isArray = p->isArray;
    addr = ob.addr;
    arraySize = ob.arraySize;
}

// Destructor for Pointer.
template <class T, int size>
Pointer<T, size>::~Pointer()
{
    typename std::list<PtrDetails<T>>::iterator p;
    p = findPtrInfo(addr);
    out_scope_num++;
    if (out_scope_num >= 10)
    {
        collect();
        p->refcount -= 10;
        out_scope_num = 0;
    }
}

// Collect garbage. Returns true if at least
// one object was freed.
template <class T, int size>
bool Pointer<T, size>::collect()
{
    bool memfreed = false;
    typename std::list<PtrDetails<T>>::iterator p;
    do
    {
        for (p = refContainer.begin(); p != refContainer.end(); p++)
        {
            if (p->refcount != 0)
                continue;

            memfreed = true;
            refContainer.erase(p);
            if (p->isArray) {
                std::cout << p->isArray << "delete by []" << "\n";
                delete[] p->memPtr;
            }
            else {
                std::cout << "delete not by []" << "\n";
                delete p->memPtr;
            }

            break;
        }
    } while (p != refContainer.end());

    return memfreed;
}

// Overload assignment of pointer to Pointer.
template <class T, int size>
T *Pointer<T, size>::operator=(T *t)
{
    typename std::list<PtrDetails<T>>::iterator p;
    p = findPtrInfo(addr);
    p->refcount--;
    PtrDetails<T> ptr(t, size);
    ptr.refcount++;
    addr = t;
    refContainer.push_back(ptr);

    return t;
}
// Overload assignment of Pointer to Pointer.
template <class T, int size>
Pointer<T, size> &Pointer<T, size>::operator=(Pointer &rv)
{
    typename std::list<PtrDetails<T>>::iterator p;
    p = findPtrInfo(addr);
    p->refcount--;

    return new Pointer<T, size>(rv);
}

// A utility function that displays refContainer.
template <class T, int size>
void Pointer<T, size>::showlist()
{
    typename std::list<PtrDetails<T>>::iterator p;
    std::cout << "refContainer<" << typeid(T).name() << ", " << size << ">:\n";
    std::cout << "memPtr refcount value\n ";
    if (refContainer.begin() == refContainer.end())
    {
        std::cout << " Container is empty!\n\n ";
    }
    for (p = refContainer.begin(); p != refContainer.end(); p++)
    {
        std::cout << "[" << (void *)p->memPtr << "]"
                  << " " << p->refcount << " ";
        if (p->memPtr)
            std::cout << " " << *p->memPtr;
        else
            std::cout << "---";
        std::cout << std::endl;
    }
    std::cout << std::endl;
}
// Find a pointer in refContainer.
template <class T, int size>
typename std::list<PtrDetails<T>>::iterator
Pointer<T, size>::findPtrInfo(T *ptr)
{
    typename std::list<PtrDetails<T>>::iterator p;
    // Find ptr in refContainer.
    for (p = refContainer.begin(); p != refContainer.end(); p++)
        if (p->memPtr == ptr)
            return p;
    return p;
}
// Clear refContainer when program exits.
template <class T, int size>
void Pointer<T, size>::shutdown()
{
    if (refContainerSize() == 0)
        return; // list is empty
    typename std::list<PtrDetails<T>>::iterator p;
    for (p = refContainer.begin(); p != refContainer.end(); p++)
    {
        // Set all reference counts to zero
        p->refcount = 0;
    }
    collect();
}

#endif // GC_POINTER_H