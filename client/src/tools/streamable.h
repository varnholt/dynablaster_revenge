#pragma once

// interface for streamable classes
// keep in mind that this adds a pointer to the virtual function table to the deriving class
// - not suitable for use in vertex structures!

class Stream;

class Streamable
{
public:
   Streamable()
   {
   }

   virtual void load(Stream *stream) = 0;
   virtual void write(Stream *stream) = 0;

   void operator << (Stream& stream)
   {
      load(&stream);
   }

   void operator >> (Stream& stream)
   {
      write(&stream);
   }
};
