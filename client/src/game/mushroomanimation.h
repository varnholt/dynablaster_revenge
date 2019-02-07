#ifndef MUSHROOMANIMATION_H
#define MUSHROOMANIMATION_H


class MushroomAnimation
{

public:

   MushroomAnimation();

   void start();

   void abort();
   bool isAborted() const;
   void setAborted(bool aborted);

   void update();

   void setTime(float time);
   float getTime() const;

   void setStartTime(float time);
   float getStartTime() const;

   void setActive(bool active);
   bool isActive() const;

   void setIntensity(float intensity);
   float getIntensity() const;

   float getNormalizedTime() const;
   
protected:


   float mTime;
   float mStartTime;

   bool mActive;

   float mIntensity;

   bool mAborted;
};

#endif // MUSHROOMANIMATION_H
