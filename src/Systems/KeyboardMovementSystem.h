#ifndef KEYBOARDMOVEMENTSYSTEM_H
#define KEYBOARDMOVEMENTSYSTEM_H

#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/KeyboardEvent.h"
#include "../Logger/Logger.h"

#include <SDL2/SDL.h>

class KeyboardMovementSystem : public System {
public:
  KeyboardMovementSystem() = default;
  void SubscribeToEvents(EventBus &eventBus) {
    auto f = [](KeyboardEvent &event) {
      Logger::Log("key " + std::string(SDL_GetKeyName(event.symbol)) +
                  " was pressed");
    };
    eventBus.subscribe<KeyboardEvent>(f);
  }
};

#endif