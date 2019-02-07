#ifndef WEIGHT_H
#define WEIGHT_H

class Stream;

class Weight
{
public:
   Weight();
   Weight(int id, float w);
   ~Weight();

   void load(Stream *stream);
   void write(Stream *stream);
   void operator << (Stream& stream);
   void operator >> (Stream& stream);

   float weight() const;
   int   id() const;

private:
   int   mID;
   float mWeight;
};

#endif
