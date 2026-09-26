#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <utility>
#include <vector>

// mimics Qt's connect()/emit shape without QObject/moc: a signal is a public member,
// .connect(callable) appends a subscriber, operator() (the old 'emit signal(...)' call site,
// now just 'signal(...)') invokes every subscriber synchronously, in registration order, on
// the calling thread - no queuing, no cross-thread marshaling.
//
// connect() returns a Connection token; disconnect(token) removes that one subscriber. Needed
// wherever a subscriber can outlive the emitting object (Qt's connect() auto-disconnected a
// destroyed QObject on both ends for free - Signal<> doesn't, so anything shorter-lived than
// the signal it subscribes to must disconnect itself, typically from its own destructor).
template <typename... Args>
class Signal
{
public:
   using Slot = std::function<void(Args...)>;
   using Connection = std::size_t;

   Connection connect(Slot slot)
   {
      const Connection id = _nextId++;
      _slots.emplace_back(id, std::move(slot));
      return id;
   }

   void disconnect(Connection id)
   {
      _slots.erase(std::remove_if(_slots.begin(), _slots.end(), [id](const auto& entry) { return entry.first == id; }), _slots.end());
   }

   void operator()(Args... args) const
   {
      // iterate a copy - a slot disconnecting another subscriber (or itself, via a deferred
      // deletion callback) while this signal is being invoked must not invalidate the loop.
      // named _subscribers, not "slots" - Qt's moc headers #define slots to nothing, which
      // would silently mangle a local variable named that into a syntax error.
      const auto _subscribers = _slots;
      for (const auto& entry : _subscribers)
      {
         entry.second(args...);
      }
   }

private:
   Connection _nextId = 0;
   std::vector<std::pair<Connection, Slot>> _slots;
};
