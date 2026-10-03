#ifndef EVENTBUS_H
#define EVENTBUS_H

#include "../ECS/ECS.h"
#include "../Logger/Logger.h"
#include "./Event.h"
#include <functional>
#include <typeindex>
#include <unordered_map>
#include <vector>

class IEventCallback {
public:
  virtual ~IEventCallback() = default;

  virtual void call(Event &e) = 0;
};

template <typename TEvent>
using CallbackFunction = std::function<void(TEvent &)>;

// contains a function as callback
template <typename TEvent> class EventCallback : public IEventCallback {
private:
  // Pointer to callback that accepts TEvent&.
  CallbackFunction<TEvent> callback;

public:
  explicit EventCallback(CallbackFunction<TEvent> cf) : callback(cf) {}

  // Call the wrapped callback with Event
  void call(Event &e) final { callback(static_cast<TEvent &>(e)); }
};

class EventBus {
  using CallbackList = std::vector<std::unique_ptr<IEventCallback>>;

  // Event type to the vector of callback functions
  std::map<std::type_index, CallbackList> subscribers{};

public:
  EventBus() = default;
  virtual ~EventBus() = default;

  void Reset() { subscribers.clear(); }

  template <typename TEvent> void subscribe(CallbackFunction<TEvent> cf) {
    auto key = std::type_index(typeid(TEvent));
    if (!(subscribers.count(key))) {
      subscribers[key] = CallbackList();
    }
    auto sub = std::make_unique<EventCallback<TEvent>>(cf);
    subscribers[key].push_back(std::move(sub));
  }

  template <typename TEvent, typename... TArgs>
  void emitEvent(TArgs &&...args) {
    auto key = std::type_index(typeid(TEvent));

    if (auto found = subscribers.find(key); found != subscribers.end()) {
      for (auto &iEventCB : found->second) {
        auto ev = TEvent{std::forward<TArgs>(args)...};
        iEventCB->call(ev);
      }
    }
  }
};

#endif