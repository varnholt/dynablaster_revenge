//! singleton class

#pragma once

#include "tools/array.h"
#include <stdio.h>

template <class Item> class Singleton
{
protected:
   //! constructor
   Singleton()
   {
      if (mInstance)
      {
         // printf("instance already exists!\n");
      }
      else
      {
         mInstance= (Item*)this;
      }
   }

   //! destructor
   virtual ~Singleton()
   {
      mInstance= 0;
   }

public:
   //! get instance
   static Item* Instance()
   {
      if (!mInstance)
         mInstance= new Item();
      return mInstance;
   }

   //! delete all instances (in back to front order)
   static void cleanUp()
   {
   }

private:
   static Item* mInstance;    //!< static instance
};

// instance is initially 0
template <class Item> Item* Singleton<Item>::mInstance = 0;
