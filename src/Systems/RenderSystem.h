#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include "../AssetStore/AssetStore.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"
#include <SDL2/SDL.h>
#include <algorithm>

class RenderSystem : public System {
public:
  RenderSystem() {
    RequireComponent<TransformComponent>();
    RequireComponent<SpriteComponent>();
  }
  void Update(SDL_Renderer *renderer, AssetStore &assetStore) {

    std::vector<Entity> &entities = GetSystemEntities();
    std::sort(entities.begin(), entities.end(), [](Entity &a, Entity &b) {
      return a.GetComponent<SpriteComponent>().zIndex <
             b.GetComponent<SpriteComponent>().zIndex;
    });
    for (auto entity : entities) {
      TransformComponent &transform = entity.GetComponent<TransformComponent>();
      SpriteComponent &sprite = entity.GetComponent<SpriteComponent>();
      SDL_Texture *texture = assetStore.GetTexture(sprite.assetId);
      if (sprite.srcRect.w == 0 || sprite.srcRect.h == 0) {
        SDL_QueryTexture(texture, NULL, NULL, &sprite.srcRect.w,
                         &sprite.srcRect.h);
        sprite.height = sprite.srcRect.h;
        sprite.width = sprite.srcRect.w;
      }
      SDL_Rect destRect = {static_cast<int>(transform.position.x),
                           static_cast<int>(transform.position.y),
                           static_cast<int>(sprite.width * transform.scale.x),
                           static_cast<int>(sprite.height * transform.scale.y)};
      SDL_RenderCopyEx(renderer, texture, &sprite.srcRect, &destRect,
                       transform.rotation, NULL, SDL_FLIP_NONE);
    }
  }
};

#endif
