#ifndef COLLISIONRENDERSYSTEM_H
#define COLLISIONRENDERSYSTEM_H

#include "../AssetStore/AssetStore.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"
#include <SDL2/SDL.h>

class CollisionRenderSystem : public System {
public:
  CollisionRenderSystem() {
    RequireComponent<TransformComponent>();
    RequireComponent<CollisionComponent>();
  }
  void Update(SDL_Renderer *renderer) {
    auto entities = GetSystemEntities();
    for (auto entity : entities) {
      TransformComponent &transform = entity.GetComponent<TransformComponent>();
      CollisionComponent &collision = entity.GetComponent<CollisionComponent>();

      const SDL_Rect rect = {static_cast<int>(transform.position.x),
                             static_cast<int>(transform.position.y),
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

#endif
