#pragma once

#include "signal.h"

#include <chrono>
#include <functional>
#include <mutex>
#include <unordered_set>

// mimics QTimer's instance API (setInterval/start/stop/isActive/timeout) plus its static
// singleShot(), backed by a process-wide registry ticked once per frame via update() - the
// caller (main.cpp's per-frame loop, or server-sdl's poll loop) decides the tick rate.
class Timer
{
public:
   Timer() = default;
   ~Timer();

   Timer(const Timer&) = delete;
   Timer& operator=(const Timer&) = delete;

   void setInterval(int milliseconds);
   void start();
   void start(int milliseconds);
   void stop();
   bool isActive() const;

   Signal<> timeoutSignal;

   static void singleShot(int milliseconds, std::function<void()> callback);
   static void update();

private:
   std::chrono::milliseconds _interval{0};
   std::chrono::steady_clock::time_point _start_time;
   bool _active = false;

   static std::mutex _mutex;
   static std::unordered_set<Timer*> _timers;
};
