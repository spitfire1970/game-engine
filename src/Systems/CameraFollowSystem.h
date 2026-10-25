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

      // can use glm::clamp but it internally does the below (helps to see the
      // order of ops)
      camera.x = std::min(std::max(camera.x, 0.0f),
                          (float)(Game::mapWidth - Game::windowWidth));
      camera.y = std::min(std::max(camera.y, 0.0f),
                          (float)(Game::mapHeight - Game::windowHeight));
    }
  }
};