/*
  Declare generic class PtrDetails
  Create generics via templates
  all attributes and methods are going to be public, because they are going to be used by other classes
  create attribute for refcount
  create attribute memPtr as generic pointer for storage of basic info about memory
  create bool attribute isArray for recognition of memory type allocated
  create attribute arraySize for working with array allocation
  Declare constructor for PtrDetails which will have two parameters
  First constructor paremeter is going to be pointer and second which 
    is not required, is going to be size of eventual array in memory
  We need to construct logic of the constructor which will set 
    isArray attribute if size is larger then 0
  Create overload operator == which will be used 
  for comparison between to PtrDetails object in lists. It is mandatory
*/
// This class defines an element that is stored
// in the garbage collection information list.
//
template <class T>
class PtrDetails
{
  public:
    unsigned refcount; // current reference count
    T *memPtr;         // pointer to allocated memory
    /* isArray is true if memPtr points
to an allocated array. It is false
otherwise. */
    bool isArray; // true if pointing to array
    /* If memPtr is pointing to an allocated
array, then arraySize contains its size */
    unsigned arraySize; // size of array
    // Here, mPtr points to the allocated memory.
    // If this is an array, then size specifies
    // the size of the array.

    PtrDetails(T *ptr, int size = 0)
    {
      memPtr = ptr;
      if (size > 0) {
	isArray = true;
      }
    }
};
// Overloading operator== allows two class objects to be compared.
// This is needed by the STL list class.
template <class T>
bool operator==(const PtrDetails<T> &ob1,
                const PtrDetails<T> &ob2)
{
  return ob1.memPtr == ob2.memPtr;
}
