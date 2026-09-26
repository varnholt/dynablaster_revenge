#pragma once

#include "frametimer.h"
#include "tools/singleton.h"

#include <QMutex>

#include <unordered_set>

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
   std::unordered_set<FrameTimer*> mTimers;
};
