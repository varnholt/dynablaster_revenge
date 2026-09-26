#include "timerhandler.h"

TimerHandler::TimerHandler()
{
}

TimerHandler::~TimerHandler()
{
   std::unordered_set<FrameTimer*>::iterator it;
   it = mTimers.begin();
   while (it != mTimers.end())
   {
      FrameTimer* timer = *it;
      it = mTimers.erase(it);
      timer->deleteLater();
   }
}

void TimerHandler::addTimer(FrameTimer* timer)
{
   if (timer)
   {
      mTimers.insert(timer);
   }
}

void TimerHandler::removeTimer(FrameTimer* timer)
{
   mTimers.erase(timer);
}

void TimerHandler::update()
{
   std::unordered_set<FrameTimer*>::iterator it = mTimers.begin();
   while (it != mTimers.end())
   {
      FrameTimer* timer = *it;

      if (timer && timer->update())
      {
         it = mTimers.erase(it);
         if (timer->mDelete)
            delete timer;
      }
      else
      {
         it++;
      }
   }
}

void TimerHandler::singleShot(float ms, QObject* receiver, const char* recvSlot)
{
   FrameTimer* timer = new FrameTimer();
   timer->setSingleShot(true);
   timer->setInterval(ms);
   timer->mDelete = true;
   timer->connect(timer, SIGNAL(timeout()), receiver, recvSlot);
   timer->start();
}
