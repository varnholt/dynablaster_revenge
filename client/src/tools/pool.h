#pragma once

#include <QHash>

template <class Item> class Pool
{
public:
   //! construct empty pool
   Pool();

   //! destructor
   virtual ~Pool();

   //! get item method
   Item* get(const char* id) const;

   //! add item
   bool add(const char* id, Item* item);

private:
   QHash<QString,Item*> mData;
};


//! construct empty pool
template <class Item> Pool<Item>::Pool()
{
}


//! destructor
template <class Item> Pool<Item>::~Pool()
{
   typename QHash<QString,Item*>::Iterator it;
   while (!mData.isEmpty())
   {
      it= mData.begin();
      Item* item= it.value();
      mData.erase(it);
      delete item;
   }
}


//! get item from pool with given "id"
template <class Item> Item* Pool<Item>::get(const char* id) const
{
   typename QHash<QString,Item*>::ConstIterator it= mData.constFind( QString(id) );
   if (it != mData.constEnd())
      return it.value();
   else
      return 0;
}


//! add item to pool with given "id"
template <class Item> bool Pool<Item>::add(const char* id, Item* item)
{
   bool result= false;
   typename QHash<QString,Item*>::ConstIterator it= mData.constFind( QString(id) );
   if (it == mData.constEnd())
   {
      mData.insert( QString(id), item );
      result= true;
   }
   return result;
}

