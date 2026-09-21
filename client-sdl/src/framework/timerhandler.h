#pragma once

#include "frametimer.h"
#include "tools/singleton.h"

#include <QMutex>
#include <QSet>

class TimerHandler : public Singleton<TimerHandler>
{
public:
   TimerHandler();
   ~TimerHandler();

   void addTimer(FrameTimer* timer);
   void removeTimer(FrameTimer* timer);

   void update();

   static void singleShot(float ms, QObject* receiver, const char* recvSlot);

private:
   mutable QMutex mMutex;
   QSet<FrameTimer*> mTimers;
};
