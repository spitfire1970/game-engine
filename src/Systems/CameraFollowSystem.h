#pragma once

#include "../Components/CameraFollowComponent.h"
#include "../Components/TransformComponent.h"
#include "../ECS/ECS.h"

class CameraFollowSystem : public System {
public:
  CameraFollowSystem() {
    RequireComponent<TransformComponent>();
    RequireComponent<CameraFollowComponent>();
  }

  void Update(glm::vec2 &camera) {
    for (auto &entity : GetSystemEntities()) {
      auto &transform = entity.GetComponent<TransformComponent>();

      camera.x = transform.position.x - (Game::windowWidth / 2);
      camera.y = transform.position.y - (Game::windowHeight / 2);
      camera.x =
          glm::clamp<float>(camera.x, 0.0, Game::mapWidth - Game::windowWidth);
      camera.y = glm::clamp<float>(camera.y, 0.0,
                                   Game::mapHeight - Game::windowHeight);
    }
  }
};