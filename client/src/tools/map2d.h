#pragma once

#include "referenced.h"
#include <memory.h>

template <class Item> class Map2d
{
public:
   //! construct empty Map2d with given size (default: zero size, null pointer)
  Map2d(int width = 0, int height = 0);

  //! construct copy of given Map2d "other"
  Map2d(const Map2d& other);

  // delete the Map2d
  virtual ~Map2d();

  //! assignment operator
  Map2d<Item>& operator = (const Map2d<Item>& list);

  // initialize Map2d to given size
  // existing data will be deallocated
  void init(int width, int height);

  // remove all elements from the Map2d.
  // capacity keeps the same, element count is set to 0
  void clear();

  // get item method
  Item* get(int x, int y) const;

  // add item
  void set(int x, int y, const Item* item);

  // return width of map
  int width() const;

  // return height of map
  int height() const;

protected:
   Item **mData;  // Map2d of items
   int  mWidth;  // size of Map2d
   int  mHeight; // size of Map2d
};


//! construct empty Map2d
template <class Item> Map2d<Item>::Map2d(int width, int height)
: mData(0)
, mWidth(width)
, mHeight(height)
{
   init(mWidth, mHeight);
}


//! construct reference of given Map2d "a"
template <class Item> Map2d<Item>::Map2d(const Map2d& a)
{
   init(a.width(), a.height());

   for (int y=0; y<a.width(); y++)
   {
      for (int x=0; x<a.width(); x++)
      {
         Item* item= a.get(x,y);
         set(x,y, item);
      }
   }
}


template <class Item> Map2d<Item>::~Map2d()
{
   delete[] mData;
}


//! assignment operator: create reference of given Map2d
template <class Item> Map2d<Item>& Map2d<Item>::operator = (const Map2d<Item>& a)
{
   init(a.width(), a.height());

   for (int y=0; y<a.height(); y++)
   {
      for (int x=0; x<a.width(); x++)
      {
         Item* item= a.get(x,y);
         set(x,y, item);
      }
   }

   return *this;
}


//! init Map2d with given size
template <class Item> void Map2d<Item>::init(int width, int height)
{
   if (mData)
      delete[] mData;

   if (width>0 && height>0)
   {
      mWidth= width;
      mHeight= height;
      mData= new Item*[width*height];
      clear();
   }
   else
   {
      mWidth= 0;
      mHeight= 0;
      mData= 0;
   }
}


//! clear Map2d
template <class Item> void Map2d<Item>::clear()
{
   for (int i=0; i<mWidth*mHeight; i++)
      mData[i]= 0;
}


//! get item from Map2d at given "index"
template <class Item> Item* Map2d<Item>::get(int x, int y) const
{
   if (x>=0 && x<mWidth && y>=0 && y<mHeight)
      return mData[y*mWidth+x];
   else
      return 0;
}


//! get item from Map2d at given "index"
template <class Item> void Map2d<Item>::set(int x, int y, const Item* item)
{
   if (x>=0 && x<mWidth && y>=0 && y<mHeight)
   {
      mData[y*mWidth+x]= (Item*)item;
   }
}


//! return number of items in the Map2d
template <class Item> int Map2d<Item>::width() const
{
   return mWidth;
}


//! return height of map
template <class Item> int Map2d<Item>::height() const
{
   return mHeight;
}

