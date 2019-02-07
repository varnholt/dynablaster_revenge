#pragma once

#include "tools/singleton.h"
#include "frametimer.h"

#include <QSet>
#include <QMutex>

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
