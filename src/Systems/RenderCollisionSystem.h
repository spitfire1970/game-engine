#pragma once

#include "../AssetStore/AssetStore.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"
#include <SDL2/SDL.h>
#include <glm/glm.hpp>

class RenderCollisionSystem : public System {
public:
  RenderCollisionSystem() {
    RequireComponent<TransformComponent>();
    RequireComponent<CollisionComponent>();
  }
  void Update(SDL_Renderer *renderer, glm::vec2 &camera) {
    auto entities = GetSystemEntities();
    for (auto entity : entities) {
      TransformComponent &transform = entity.GetComponent<TransformComponent>();
      CollisionComponent &collision = entity.GetComponent<CollisionComponent>();

      const SDL_Rect rect = {
          static_cast<int>(transform.position.x -
                           (transform.isFixed ? 0 : camera.x)),
          static_cast<int>(transform.position.y -
                           (transform.isFixed ? 0 : camera.y)),
          collision.width, collision.height};
      SDL_SetRenderDrawColor(
          renderer, 255,
          (!entity.GetComponent<CollisionComponent>().isColliding) * 255, 0,
          255);
      SDL_RenderDrawRect(renderer, &rect);
    }

    for (auto &entity : entities) {
      entity.GetComponent<CollisionComponent>().isColliding = false;
    }
  }
};
