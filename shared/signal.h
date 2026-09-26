#pragma once

#include <functional>
#include <vector>

// mimics Qt's connect()/emit shape without QObject/moc: a signal is a public member,
// .connect(callable) appends a subscriber, operator() (the old 'emit signal(...)' call site,
// now just 'signal(...)') invokes every subscriber synchronously, in registration order, on
// the calling thread - no queuing, no cross-thread marshaling
template <typename... Args>
class Signal
{
public:
   using Slot = std::function<void(Args...)>;

   void connect(Slot slot)
   {
      _slots.push_back(std::move(slot));
   }

   void operator()(Args... args) const
   {
      for (const auto& slot : _slots)
      {
         slot(args...);
      }
   }

private:
   std::vector<Slot> _slots;
};
