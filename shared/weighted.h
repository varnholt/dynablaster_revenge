#ifndef WEIGHTED_H
#define WEIGHTED_H

template<typename O, typename W>
class Weighted
{

public:

   Weighted()
   {
   }

   Weighted(O object, W weight)
    : mObject(object),
      mWeight(weight)
   {

   }

   bool operator == (const Weighted& other) const
   {
      return other.mWeight == mWeight;
   }

   bool operator < (const Weighted& other) const
   {
      return other.mWeight < mWeight;
   }

   O getObject() const
   {
      return mObject;
   }

   W getWeight() const
   {
      return mWeight;
   }

   void setWeight(W weight)
   {
       mWeight = weight;
   }


protected:

   O mObject;
   W mWeight;
};

#endif // WEIGHTED_H



