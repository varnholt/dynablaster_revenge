#include "timerhandler.h"
#include <QMutexLocker>

TimerHandler::TimerHandler()
{
}

TimerHandler::~TimerHandler()
{
   QMutexLocker lock(&mMutex);
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
      QMutexLocker lock(&mMutex);
      mTimers.insert(timer);
   }
}

void TimerHandler::removeTimer(FrameTimer* timer)
{
   QMutexLocker lock(&mMutex);
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
         mMutex.lock();
         it = mTimers.erase(it);
         mMutex.unlock();
         if (timer->mDelete)
            delete timer;
      }
      else
      {
         mMutex.lock();
         it++;
         mMutex.unlock();
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
