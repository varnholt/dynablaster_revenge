// frame counter class
// register any rendered frame with next(),
// get the current framerate with get().
// specify an averaging period (in number of frames) in the constructor

#ifndef FPS_H
#define FPS_H

class FPS
{
private:
   int   mPeriod;
   float mCurFPS;
   int   mFrame;
   int   mTime;

public:
   FPS(int period=100);
   ~FPS();
   void next();
   float get();
};

#endif
