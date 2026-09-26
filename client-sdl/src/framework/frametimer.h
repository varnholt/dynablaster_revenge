#pragma once

#include "signal.h"

class FrameTimer
{
public:
   FrameTimer();
   FrameTimer(const FrameTimer& other);
   ~FrameTimer();

   FrameTimer& operator=(const FrameTimer& other);

   static FrameTimer currentTime();

   bool isValid() const;

   FrameTimer addMSecs(float ms) const;
   float msecsTo(const FrameTimer& other) const;

   void start();
   void start(float interval);
   void restart();
   void stop();

   float elapsed() const;

   bool update();

   float interval() const;
   void setInterval(float ms);

   void setSingleShot(bool singleShot);

   Signal<> timeoutSignal;

   bool mStarted;
   bool mSingleShot;
   float mStartTime;
   float mInterval;
   bool mDelete;
};
