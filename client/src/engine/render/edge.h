#ifndef EDGE_H
#define EDGE_H

class Stream;

class Edge
{
public:
   void operator << (Stream& stream);

   unsigned short i1, i2;
   unsigned short i3, i4;
   int f1,f2;
   int flags;
};

#endif
