#include "timer.h"

#include <algorithm>
#include <vector>

namespace
{
struct PendingSingleShot
{
   std::chrono::steady_clock::time_point due;
   std::function<void()> callback;
};

std::mutex _single_shot_mutex;
std::vector<PendingSingleShot> _pending_single_shots;
}  // namespace

std::mutex Timer::_mutex;
std::unordered_set<Timer*> Timer::_timers;

Timer::~Timer()
{
   std::lock_guard<std::mutex> lock(_mutex);
   _timers.erase(this);
}

void Timer::setInterval(int milliseconds)
{
   _interval = std::chrono::milliseconds(milliseconds);
}

int Timer::interval() const
{
   return static_cast<int>(_interval.count());
}

void Timer::start()
{
   _start_time = std::chrono::steady_clock::now();
   _active = true;

   std::lock_guard<std::mutex> lock(_mutex);
   _timers.insert(this);
}

void Timer::start(int milliseconds)
{
   setInterval(milliseconds);
   start();
}

void Timer::stop()
{
   _active = false;

   std::lock_guard<std::mutex> lock(_mutex);
   _timers.erase(this);
}

bool Timer::isActive() const
{
   return _active;
}

void Timer::singleShot(int milliseconds, std::function<void()> callback)
{
   std::lock_guard<std::mutex> lock(_single_shot_mutex);
   _pending_single_shots.push_back({std::chrono::steady_clock::now() + std::chrono::milliseconds(milliseconds), std::move(callback)});
}

void Timer::update()
{
   const auto now = std::chrono::steady_clock::now();

   std::vector<Timer*> due;
   {
      std::lock_guard<std::mutex> lock(_mutex);
      for (auto* timer : _timers)
      {
         if (now - timer->_start_time >= timer->_interval)
         {
            due.push_back(timer);
         }
      }
   }

   for (auto* timer : due)
   {
      timer->_start_time = now;
      timer->timeoutSignal();
   }

   std::vector<std::function<void()>> callbacks;
   {
      std::lock_guard<std::mutex> lock(_single_shot_mutex);
      auto it = std::remove_if(
         _pending_single_shots.begin(),
         _pending_single_shots.end(),
         [now, &callbacks](auto& pending)
         {
            if (now < pending.due)
            {
               return false;
            }
            callbacks.push_back(std::move(pending.callback));
            return true;
         }
      );
      _pending_single_shots.erase(it, _pending_single_shots.end());
   }

   for (auto& callback : callbacks)
   {
      callback();
   }
}
