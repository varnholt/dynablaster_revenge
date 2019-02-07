/*
 growable array template with reference counting
 usually the number of stored elements is known in advance and the array will never grow
 whenever the array needs to resize it allocates a small bonus capacity to avoid
 permanent resizing when adding multiple elements
 initially the array has no space allocated (null pointer)

 usage:
 array.init(...) with the number of items required
 array.add(...) items
 array.get(..) item from the array
*/

#ifndef ARRAY_H
#define ARRAY_H

#include "referenced.h"
#include <memory.h>

template <class Item> class Array : public Referenced
{
  #define GROWARRAY 10

public:
   //! construct empty array with given size (default: zero size, null pointer)
  Array(int size = 0);

  //! construct reference of given array "a"
  Array(const Array& a);

  //! construct new array from given items and count
  Array(Item* items, int count);

  // delete the array
  virtual ~Array();

  //! assignment operator: reference given array
  Array<Item>& operator = (const Array<Item>& list);

  // initialize array to given size
  // existing data will be deallocated
  void init(int size);

  // remove all elements from the array. 
  // capacity keeps the same, element count is set to 0
  void clear();

  // get item operator (inline)
  Item& operator[](int index) const
  {
     return mData[index];
  }

  // get item method
  const Item& get(int index) const;

  // add item
  int add(const Item& item);

  // add list
  int add(const Array<Item>& data);

  // get last element of array
  const Item& getLast() const;

  // get and remove last item
  const Item& takeLast();

  //! return index of element (-1 if not existing)
  int indexOf(const Item& item) const;

  //! return whether "item" exists in the array
  bool contains(const Item& item) const;

  // erase item at given index. following items will be moved
  void erase(int index);

  // create a copy of given array "a"
  void copy(const Array<Item>& a);

  // remove given item. last item will be placed on the position of the erased item (order changes!)
  // non-pointer types need "==" operator
  bool remove(const Item& item);

  // get array pointer
  Item* data() const;

  // return number of items currently in the array
  int size() const;

  // return number of items currently in the array
  int capacity() const;

private:
  // create a deep copy of given data
  void deepCopy(const Array<Item>& a);

protected:
   Item *mData; // array of items
   int  mSize;  // size of array
   int  mCount; // number of items in array
};

//! construct empty array
template <class Item> Array<Item>::Array(int size)
: Referenced()
, mData(0)
, mSize(size)
, mCount(0)
{
   if (mSize>0)
      mData= new Item[mSize];
}

//! construct reference of given array "a"
template <class Item> Array<Item>::Array(const Array& a)
: Referenced(a)
{
   mData= a.data();
   mSize= a.capacity();
   mCount= a.size();
}

//! construct array with given items and count
template <class Item> Array<Item>::Array(Item* items, int count)
: Referenced()
, mData(0)
, mSize(count)
, mCount(count)
{
   if (count>0)
   {
      mData= new Item[count];
      for (int i=0;i<count;i++)
         mData[i]= items[i];
   }
}

//! destructor: delete array if no more references
template <class Item> Array<Item>::~Array()
{
   if (getRefCount()==1)
   {
      if (mData) delete[] mData;
   }
}

//! assignment operator: create reference of given array
template <class Item> Array<Item>& Array<Item>::operator = (const Array<Item>& list)
{
   if (this != &list)
   {
      // array is not referenced: delete data and reference counter
      if (getRefCount() == 1)
      {
         if (mData)
            delete[] mData;
         delete mReferences;
      }

      mReferences= list.getRef();
      addRef();
      mSize= list.capacity();
      mCount= list.size();
      mData= list.data();
   }
   return *this;
}

//! init array with given size
template <class Item> void Array<Item>::init(int size)
{
   // data is not referenced by another object? delete it.
   if (!copyRef())
   {
      if (mData)
         delete[] mData;
   }

   if (size>0)
      mData= new Item[size];
   else
      mData= 0;
   mSize= size;
   mCount= 0;
}

//! clear array
template <class Item> void Array<Item>::clear()
{
   if (copyRef())
      mData= new Item[mSize];
   mCount= 0;
}

//! get item from array at given "index"
template <class Item> const Item& Array<Item>::get(int index) const
{
   return mData[index];
}


//! get and remove last item
template <class Item> const Item& Array<Item>::getLast() const
{
   return mData[mCount-1];
}


//! get and remove last item
template <class Item> const Item& Array<Item>::takeLast()
{
   mCount--;
   return mData[mCount];
}


//! add item at the end of the array
template <class Item> int Array<Item>::add(const Item& item)
{
   if (copyRef())
      deepCopy(*this);
   else
   {
      // grow array when more elements are needed
      if (mCount>=mSize)
      {
         Item *temp= mData;
         mSize+=GROWARRAY; 
         mData= new Item[mSize];
         if (temp)
         {
            for (int i=0;i<mCount;i++) mData[i]= temp[i];
            delete[] temp;
         }
      }
   }
   int index= mCount;
   mData[mCount]= item;
   mCount++;
   return index;
}


//! add content of other array
template <class Item> int Array<Item>::add(const Array<Item>& data)
{
   if (copyRef())
      deepCopy(*this);
   else
   {
      // grow array when more elements are needed
      if (mCount + data.size() >= mSize)
      {
         Item *temp= mData;
         mSize= mCount + data.size();
         mData= new Item[mSize];
         if (temp)
         {
            for (int i=0; i<mCount; i++)
               mData[i]= temp[i];
            delete[] temp;
         }
      }
   }

   for (int i=0; i<data.size(); i++)
      mData[mCount+i]= data[i];

   mCount+= data.size();

   return -1;
}

//! return index of element (-1 if not existing)
template <class Item> int Array<Item>::indexOf(const Item& item) const
{
   for (int index=0; index<mCount; index++)
   {
      if (mData[index] == item)
         return index;
   }
   return -1;
}

//! return whether given element is in the list
template <class Item> bool Array<Item>::contains(const Item& item) const
{
   for (int index=0; index<mCount; index++)
   {
      if (mData[index] == item)
         return true;
   }
   return false;
}

//! remove element at "index"
template <class Item> void Array<Item>::erase(int index)
{
   if (copyRef())
   {
      //! create new array without element at index
      Item *temp= mData;
      mData= new Item[mSize];
      for (int i=0;i<index;i++)
         mData[i]= temp[i];
      for (int i=index;i<mCount-1;i++)
         mData[i]= temp[i+1];
   }
   else
   {
      // copy in-place
      for (int i=index;i<mCount-1;i++)
         mData[i]= mData[i+1];
   }

   mCount--;
}

//! create a copy of the given array "a"
template <class Item> void Array<Item>::copy(const Array<Item>& a)
{
   if (!copyRef())
   {
      if (mData)
         delete[] mData;
   }

   mCount= mSize= a.size();
   if (mCount > 0)
   {
      mData= new Item[mCount];
      for (int i=0;i<mCount;i++)
         mData[i]= a[i];
   }
   else
      mData= 0;
}

//! remove all occurences of "item" from the array
//! returns true if items were removed
template <class Item> bool Array<Item>::remove(const Item& item)
{
   if (copyRef())
      deepCopy(*this);

   int pos= 0;
   for (int i=0;i<mCount;i++)
   {
      if (mData[i] != item)
      {
         if (pos != i)
            mData[pos]= mData[i];
         pos++;
      }

   }
   bool result= (mCount != pos);
   mCount= pos;
   return result;
}

//! return pointer to array-data
template <class Item> Item* Array<Item>::data() const
{
   return mData;
}

// return number of items in the array
template <class Item> int Array<Item>::size() const
{
   return mCount;
}

// return number of items the array can hold without reallocating
template <class Item> int Array<Item>::capacity() const
{
   return mSize;
}

//! create a deep copy of the given array
template <class Item> void Array<Item>::deepCopy(const Array<Item>& a)
{
   Item *temp= a.data();
   mCount= a.size();
   mSize= mCount + GROWARRAY;
   mData= new Item[mSize];
   for (int i=0;i<mCount;i++)
      mData[i]= temp[i];
}

#endif
