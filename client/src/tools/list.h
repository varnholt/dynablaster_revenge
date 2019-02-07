// template list class. adds load functionality to array<>
#pragma once

#include "array.h"
#include "streamable.h"
#include "stream.h"

template <class Item> class List : public Array<Item>, public Streamable
{
public:
   List()
   : Array<Item>()
   {
   }

   List(Item *items, int count)
   : Array<Item>(items, count)
   {
   }

   virtual ~List()
   {
   }

/*
   List<Item>& operator = (const List<Item>& list)
   {
      init( list.size() );
      for (int i=0;i<list.size();i++)
         add(list[i]);
      return *this;
   }
*/

   // load array from stream
   virtual void load(Stream *stream)
   {
      this->copyRef();

      // get number of items
      int size= stream->getInt();

      // no items: NULL
      if (size==0)
      {
         Array<Item>::init(size);
         return;
      }

      // current array not big enough (or not yet initialized): create new
      if (size>this->mSize)
      {
         if (this->mData) delete this->mData;
         this->mData= new Item[size];
         this->mSize= size;
      }

      // load items
      for (int i=0;i<size;i++)
      {
         Item& item= this->mData[i];
         item << *stream;
      }

      this->mCount= size;
   }

   // load array from stream
   virtual void write(Stream *stream)
   {
      stream->writeInt( this->size() );

      // load items
      for (int i=0; i<this->size(); i++)
      {
         Item& item= this->mData[i];
         item >> *stream;
      }
   }

};
