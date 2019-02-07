#ifndef SHROOMFILTER_H
#define SHROOMFILTER_H

#include "filter.h"

class ShroomFilter : public Filter
{
public:

   ShroomFilter();
   virtual ~ShroomFilter();

   virtual bool init();
   virtual void process(unsigned int texture, float u, float v);

   const String& getName() const;

   void setIntensity(float intensity);
   float getIntensity() const;
   int getIntensityParam() const;

   void setTime(float time);
   float getTime() const;
   int getTimeParam() const;


protected:

   unsigned int mShader;

   int mIntensityParam;
   float mIntensity;

   int mTimeParam;
   float mTime;

   int mTextureParam;
};

#endif // SHROOMFILTER_H
