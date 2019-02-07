#include "referenced.h"

Referenced::Referenced()
: mReferences(new int(1))
{
}

Referenced::Referenced(const Referenced& r)
: mReferences( r.getRef() )
{
   addRef();
}

Referenced::Referenced(const Referenced* r)
: mReferences( r->getRef() )
{
   addRef();
}

Referenced::~Referenced()
{
   if (!deref())
   {
      if (mReferences)
      {
         delete mReferences;
         mReferences= 0;
      }
   }
}

void Referenced::addRef() const
{
   int count= (*mReferences) + 1;
   *mReferences = count;
}

bool Referenced::deref()
{
   int count= (*mReferences) - 1;
   *mReferences = count;
   return (count!=0);
}

bool Referenced::copyRef()
{
   if (*mReferences > 1)
   {
      *mReferences = (*mReferences)-1;
      mReferences= new int(1);
      return true;
   }
   else
      return false;
}

int Referenced::getRefCount() const
{
   return *mReferences;
}

int* Referenced::getRef() const
{
   return mReferences;
}

