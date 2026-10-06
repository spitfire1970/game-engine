#pragma once

#include "../Components/CameraFollowComponent.h"
#include "../Components/CollisionComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"
#include "../Logger/Logger.h"
#include <SDL2/SDL.h>

class ProjectileEmitterSystem : public System {
public:
  ProjectileEmitterSystem() {
    RequireComponent<ProjectileEmitterComponent>();
    RequireComponent<TransformComponent>();
  };

  void SubscribeToEvents(EventBus &eventBus) {
    auto f = [this](KeyboardEvent &event) { ShootManual(event); };
    eventBus.subscribe<KeyboardEvent>(f);
  }

  void ShootManual(KeyboardEvent &event) {
    if (event.symbol != SDLK_SPACE)
      return;
    for (auto &entity : GetSystemEntities()) {
      auto &projectileEmitter =
          entity.GetComponent<ProjectileEmitterComponent>();
      if (!(entity.HasComponent<CameraFollowComponent>())) {
        continue;
      }
      auto &rigidBody = entity.GetComponent<RigidBodyComponent>();
      glm::vec2 d = glm::vec2(0);
      if (rigidBody.velocity.x > 0)
        d.x = 1;
      if (rigidBody.velocity.x < 0)
        d.x = -1;
      if (rigidBody.velocity.y > 0)
        d.y = 1;
      if (rigidBody.velocity.y < 0)
        d.y = -1;
      projectileEmitter.projectileVelocity = glm::vec2(
          projectileEmitter.speed * d.x, projectileEmitter.speed * d.y);
      projectileEmitter.lastEmitTime = -1 * projectileEmitter.frequency;
    }
  }

  void Update() {
    for (auto &entity : GetSystemEntities()) {
      auto &projectileEmitter =
          entity.GetComponent<ProjectileEmitterComponent>();
      auto &transform = entity.GetComponent<TransformComponent>();

      if (SDL_GetTicks() - projectileEmitter.lastEmitTime >
          projectileEmitter.frequency) {
        glm::vec2 projectilePosition = transform.position;
        if (entity.HasComponent<SpriteComponent>()) {
          const auto &sprite = entity.GetComponent<SpriteComponent>();
          projectilePosition.x += (transform.scale.x * sprite.width / 2);
          projectilePosition.y += (transform.scale.y * sprite.height / 2);
        }
        auto e = entity.registry->CreateEntity();
        e.Group("projectiles");
        e.AddComponent<TransformComponent>(projectilePosition);
        e.AddComponent<RigidBodyComponent>(
            projectileEmitter.projectileVelocity);
        e.AddComponent<SpriteComponent>("bullet-image", 4, 4, 4);
        e.AddComponent<CollisionComponent>(4, 4);
        e.AddComponent<ProjectileComponent>(
            entity.HasTag("player"), projectileEmitter.hitPercentDamage,
            projectileEmitter.projectileLifecycleDuration);
        projectileEmitter.lastEmitTime = SDL_GetTicks();
      }
    }
  }
};