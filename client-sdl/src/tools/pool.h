#pragma once

#include <string>
#include <unordered_map>

template <class Item>
class Pool
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
   std::unordered_map<std::string, Item*> mData;
};

//! construct empty pool
template <class Item>
Pool<Item>::Pool()
{
}

//! destructor
template <class Item>
Pool<Item>::~Pool()
{
   for (auto& [id, item] : mData)
   {
      delete item;
   }
   mData.clear();
}

//! get item from pool with given "id"
template <class Item>
Item* Pool<Item>::get(const char* id) const
{
   auto it = mData.find(id);
   if (it != mData.end())
      return it->second;
   else
      return 0;
}

//! add item to pool with given "id"
template <class Item>
bool Pool<Item>::add(const char* id, Item* item)
{
   bool result = false;
   auto it = mData.find(id);
   if (it == mData.end())
   {
      mData.insert({std::string(id), item});
      result = true;
   }
   return result;
}
