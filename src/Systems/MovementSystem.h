#ifndef MOVEMENTSYSTEM_H
#define MOVEMENTSYSTEM_H

#include "../Components/RigidBodyComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"
#include <glm/glm.hpp>

class MovementSystem : public System {
public:
  MovementSystem() {
    RequireComponent<TransformComponent>();
    RequireComponent<RigidBodyComponent>();
  }

  void SubscribeToEvents(EventBus &eventBus) {
    auto f = [this](CollisionEvent &event) { OnCollision(event); };
    eventBus.subscribe<CollisionEvent>(f);
  }

  void OnCollision(CollisionEvent &event) {
    Entity a = event.a;
    Entity b = event.b;

    if (a.BelongsToGroup("obstacles") && b.BelongsToGroup("enemies")) {
      OnObstacleHit(b);
    }

    if (b.BelongsToGroup("obstacles") && a.BelongsToGroup("enemies")) {
      OnObstacleHit(a);
    }
  }
  // TODO: the shadows get messed up so using SDL_FLIP_HORIZONTAL is probably
  // not even what we want
  void OnObstacleHit(Entity enemy) {
    if (enemy.HasComponent<RigidBodyComponent>()) {
      auto &rigidBody = enemy.GetComponent<RigidBodyComponent>();
      const glm::vec2 vel = rigidBody.velocity;

      if (vel.x != 0) {
        rigidBody.velocity.x = vel.x * -1;
        if (enemy.HasComponent<SpriteComponent>()) {
          auto &sprite = enemy.GetComponent<SpriteComponent>();
          const SDL_RendererFlip flip = sprite.flip;
          // horizontal
          // flipped?
          if ((flip & SDL_FLIP_HORIZONTAL) == SDL_FLIP_HORIZONTAL) {
            sprite.flip =
                (SDL_RendererFlip)(sprite.flip & (~SDL_FLIP_HORIZONTAL));
          } else {
            sprite.flip = (SDL_RendererFlip)(sprite.flip | SDL_FLIP_HORIZONTAL);
          }
        }
      }

      // TODO: change this to SDL_FLIP_VERTICAL later?
      if (vel.y != 0) {
        rigidBody.velocity.y = vel.y * -1;
        if (enemy.HasComponent<SpriteComponent>()) {
          auto &sprite = enemy.GetComponent<SpriteComponent>();

          const SDL_RendererFlip flip = sprite.flip;
          // vertical
          // flipped?
          if ((flip & SDL_FLIP_HORIZONTAL) == SDL_FLIP_HORIZONTAL) {
            sprite.flip =
                (SDL_RendererFlip)(sprite.flip & (~SDL_FLIP_HORIZONTAL));
          } else {
            sprite.flip = (SDL_RendererFlip)(sprite.flip | SDL_FLIP_HORIZONTAL);
          }
        }
      }
    }
  };

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
