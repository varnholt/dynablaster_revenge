#include "frametimer.h"
#include "globaltime.h"
#include "timerhandler.h"

FrameTimer::FrameTimer()
: QObject()
, mStarted(false)
, mSingleShot(false)
, mStartTime(0.0f)
, mInterval(0.0f)
, mDelete(false)
{
}

FrameTimer::FrameTimer(QObject* parent)
: QObject(parent)
, mStarted(false)
, mSingleShot(false)
, mStartTime(0.0f)
, mInterval(0.0f)
, mDelete(false)
{
}

FrameTimer::FrameTimer(const FrameTimer& other)
: QObject()
, mStarted( other.mStarted )
, mSingleShot( other.mSingleShot )
, mStartTime( other.mStartTime )
, mInterval( other.mInterval )
, mDelete(false)
{
   if (mStarted)
      TimerHandler::Instance()->addTimer( this );
}

FrameTimer::~FrameTimer()
{
   if (mStarted)
      TimerHandler::Instance()->removeTimer( this );
}

FrameTimer& FrameTimer::operator = (const FrameTimer& other)
{
   if (&other != this)
   {
      mStarted= other.mStarted;
      mSingleShot= other.mSingleShot;
      mStartTime= other.mStartTime;
      mInterval = other.mInterval;
      if (mStarted)
         TimerHandler::Instance()->addTimer( this );
   }
   return *this;
}

FrameTimer FrameTimer::currentTime()
{
   FrameTimer time;
   time.mStartTime= GlobalTime::Instance()->getTime();
   return time;
}

bool FrameTimer::isValid() const
{
   return mStarted;
}

void FrameTimer::setSingleShot(bool singleShot)
{
   mSingleShot= singleShot;
}

FrameTimer FrameTimer::addMSecs(float ms) const
{
   FrameTimer time;
   time.mStartTime= mStartTime + ms * 0.001f;
   return time;
}

float FrameTimer::msecsTo(const FrameTimer& other) const
{
   return (other.mStartTime - mStartTime) * 1000.0f;
}

void FrameTimer::setInterval(float interval)
{
   mInterval= interval * 0.001f;
}

float FrameTimer::interval() const
{
   return mInterval * 1000.0f;
}

void FrameTimer::start()
{
   mStartTime= GlobalTime::Instance()->getTime();
   if (!mStarted)
   {
      mStarted= true;
      TimerHandler::Instance()->addTimer( this );
   }
}

void FrameTimer::start(float interval)
{
   setInterval(interval);
   start();
}

void FrameTimer::restart()
{
   start();
}

void FrameTimer::stop()
{
   if (mStarted)
   {
      mStarted= false;
      TimerHandler::Instance()->removeTimer( this );
   }
}

float FrameTimer::elapsed() const
{
   if (mStarted)
   {
      float curTime= GlobalTime::Instance()->getTime();
      return (curTime - mStartTime) * 1000.0f;
   }
   else
      return 0.0f;
}

bool FrameTimer::update()
{
   if (mStarted && mInterval > 0.0f)
   {
      float curTime= GlobalTime::Instance()->getTime();
      if (curTime >= mStartTime + mInterval)
      {
         emit timeout();
         if (mSingleShot)
         {
            mStarted= false;
            return true;
         }
         else
            mStartTime= curTime;
      }
   }
   return false;
}

