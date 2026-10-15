#pragma once

#include "../Components/HealthComponent.h"
#include "../Components/TextLabelComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"
#include "SDL2/SDL.h"
#include <glm/glm.hpp>

class RenderHealthSystem : public System {
public:
  RenderHealthSystem() {
    RequireComponent<TransformComponent>();
    RequireComponent<HealthComponent>();
    RequireComponent<TextLabelComponent>();
  }

  void Update(SDL_Renderer *renderer, AssetStore &assetStore,
              glm::vec2 &camera) {
    const auto &entities = GetSystemEntities();
    for (const auto &entity : entities) {
      const auto &transform = entity.GetComponent<TransformComponent>();
      const auto &health = entity.GetComponent<HealthComponent>();
      auto &textLabel = entity.GetComponent<TextLabelComponent>();
      textLabel.assetId = "arial-font-8";
      // text, assetId, color, offset

      SDL_Color color;

      if (health.healthPercentage <= 20) {
        // red
        color = {255, 0, 0};
      } else if (health.healthPercentage >= 70) {
        // green
        color = {0, 255, 0};
      } else {
        // yellow?
        color = {255, 255, 0};
      }
      textLabel.color = color;
      textLabel.text = std::to_string(health.healthPercentage) + "%";
      textLabel.offset = glm::vec2(0, -16);

      const SDL_Rect rect = {
          static_cast<int>(transform.position.x -
                           (transform.isFixed ? 0 : camera.x) - 2),
          static_cast<int>(transform.position.y -
                           (transform.isFixed ? 0 : camera.y) - 5),
          static_cast<int>(health.healthPercentage * 0.3), 3};
      SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
      SDL_RenderFillRect(renderer, &rect);
    }
  }
};
