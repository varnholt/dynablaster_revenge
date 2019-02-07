#pragma once

// do not derive from Streamable

class Stream;

class UV
{
public:
   UV();
   UV(float su, float sv);

   operator const float* () const { return &u; }   //! cast to float*

   void operator << (Stream& stream);
   void operator >> (Stream& stream);

   void load(Stream *stream);
   void write(Stream *stream);

   float u,v;
};
