#pragma once

#include "frametimer.h"
#include "tools/singleton.h"

#include <functional>
#include <unordered_set>

class TimerHandler : public Singleton<TimerHandler>
{
public:
   TimerHandler();
   ~TimerHandler();

   void addTimer(FrameTimer* timer);
   void removeTimer(FrameTimer* timer);

   void update();

   static void singleShot(float ms, std::function<void()> callback);

private:
   std::unordered_set<FrameTimer*> mTimers;
};
