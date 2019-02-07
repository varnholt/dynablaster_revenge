// this is the base class for a key-frame and contains the non-template part of "key" (a timestamp)

#include "tools/streamable.h"

class KeyBase : public Streamable
{
public:
   KeyBase(int time = 0);
   virtual ~KeyBase();

   int time() const;

   virtual void load(Stream *stream);
   virtual void write(Stream *stream);

protected:
   int mTime;
};
