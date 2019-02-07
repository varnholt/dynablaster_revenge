#include "objectname.h"

ObjectName::ObjectName()
{
}

ObjectName::ObjectName(const ObjectName& other)
: mName(other.name())
{
}

ObjectName::ObjectName( const String& name )
: mName(name)
{
}

ObjectName::~ObjectName()
{
}

const String& ObjectName::name() const
{
   return mName;
}

void ObjectName::setName(const String& name)
{
   mName= name;
}

void ObjectName::load(Stream* stream)
{
   mName.load(stream);
}

void ObjectName::write(Stream* stream)
{
   mName.write(stream);
}
