// template class to represent a single animation key of given template type

#pragma once

#include "keybase.h"
#include "tools/stream.h"

template <class Item> 
class Key : public KeyBase
{
public:
   Key();
   Key(int time, const Item& value);

   const Item&  value() const;
   void         setValue(const Item& v);

   virtual void load(Stream *stream);
   virtual void write(Stream *stream);

protected:
   Item mValue;
};

template <class Item> 
Key<Item>::Key()
: KeyBase(0)
{
}

template <class Item> 
Key<Item>::Key(int time, const Item& value)
: KeyBase(time)
, mValue(value)
{
}

template <class Item> 
const Item& Key<Item>::value() const
{
   return mValue;
}

template <class Item> 
void Key<Item>::setValue(const Item& v)
{
   mValue= v;
}

template <class Item> 
void Key<Item>::load(Stream *stream)
{
   KeyBase::load(stream);
   mValue << *stream;
}

template <class Item> 
void Key<Item>::write(Stream *stream)
{
   KeyBase::write(stream);
   mValue >> *stream;
}
