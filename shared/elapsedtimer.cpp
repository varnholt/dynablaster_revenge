#include "elapsedtimer.h"

ElapsedTimer::ElapsedTimer() : _start_time(std::chrono::steady_clock::now())
{
}

void ElapsedTimer::start()
{
   _start_time = std::chrono::steady_clock::now();
}

int64_t ElapsedTimer::restart()
{
   const auto now = std::chrono::steady_clock::now();
   const auto elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - _start_time).count();
   _start_time = now;
   return elapsed_ms;
}

int64_t ElapsedTimer::elapsed() const
{
   return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - _start_time).count();
}
