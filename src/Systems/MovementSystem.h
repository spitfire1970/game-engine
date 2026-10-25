#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "../Components/RigidBodyComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"

class MovementSystem : public System {
public:
  MovementSystem() {
    RequireComponent<TransformComponent>();
    RequireComponent<RigidBodyComponent>();
  }
  void Update(double deltaTime) {
    for (auto entity : GetSystemEntities()) {
      TransformComponent &transform = entity.GetComponent<TransformComponent>();
      const RigidBodyComponent &rigidBody =
          entity.GetComponent<RigidBodyComponent>();
      transform.position.x += rigidBody.velocity.x * deltaTime;
      transform.position.y += rigidBody.velocity.y * deltaTime;

      bool isEntityOutsideMap =
          (transform.position.x > Game::mapWidth ||
           transform.position.y > Game::mapHeight || transform.position.x < 0 ||
           transform.position.y < 0);

      if (isEntityOutsideMap && !entity.HasTag("player")) {
        entity.Kill();
      }
    }
  }
};

#endif
