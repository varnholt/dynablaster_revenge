#pragma once

#include "string.h"

class ObjectName
{
public:
   ObjectName();
   ObjectName(const ObjectName& other);
   ObjectName( const String& name );
   virtual ~ObjectName();

   const String& name() const;
   void setName(const String& name);

   void load(Stream* stream);
   void write(Stream* stream);

protected:
   String mName;
};
