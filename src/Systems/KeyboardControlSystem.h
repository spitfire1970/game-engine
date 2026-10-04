#pragma once

#include "../Components/KeyboardControlComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/KeyboardEvent.h"

class KeyboardControlSystem : public System {
public:
  KeyboardControlSystem() {
    RequireComponent<SpriteComponent>();
    RequireComponent<RigidBodyComponent>();
    RequireComponent<KeyboardControlComponent>();
  }

  void SubscribeToEvents(EventBus &eventBus) {
    auto f = [this](KeyboardEvent &event) { ChangeSpeedAndSprite(event); };
    eventBus.subscribe<KeyboardEvent>(f);
  }

  void ChangeSpeedAndSprite(KeyboardEvent &event) {
    for (auto &entity : GetSystemEntities()) {
      auto &sprite = entity.GetComponent<SpriteComponent>();
      auto &rigidBody = entity.GetComponent<RigidBodyComponent>();
      auto &keyboardControl = entity.GetComponent<KeyboardControlComponent>();
      if (event.symbol == keyboardControl.upKey) {
        rigidBody.velocity.y = -1 * keyboardControl.speed;
        rigidBody.velocity.x = 0;
        sprite.srcRect.y = 0 * sprite.height;
      } else if (event.symbol == keyboardControl.rightKey) {
        rigidBody.velocity.x = keyboardControl.speed;
        rigidBody.velocity.y = 0;
        sprite.srcRect.y = 1 * sprite.height;
      } else if (event.symbol == keyboardControl.downKey) {
        rigidBody.velocity.y = keyboardControl.speed;
        rigidBody.velocity.x = 0;
        sprite.srcRect.y = 2 * sprite.height;
      } else if (event.symbol == keyboardControl.leftKey) {
        rigidBody.velocity.x = -1 * keyboardControl.speed;
        rigidBody.velocity.y = 0;
        sprite.srcRect.y = 3 * sprite.height;
      }
    }
  }
};