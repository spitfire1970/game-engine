#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "../ECS/ECS.h"
#include "../Logger/Logger.h"

bool checkAABBCollision(float ax, float ay, int aw, int ah, float bx, float by,
                        int bw, int bh) {
  return ((ax + aw >= bx) && (bx + bw >= ax) && (ay + ah >= by) &&
          (by + bh >= ay));
}

class CollisionSystem : public System {
public:
  CollisionSystem() {
    RequireComponent<TransformComponent>();
    RequireComponent<CollisionComponent>();
  }
  void Update() {
    Logger::Log("here");
    auto entities = GetSystemEntities();
    Logger::Log("here1");
    for (auto i = entities.begin(); i < entities.end(); i++) {
      for (auto j = i + 1; j < entities.begin(); j++) {
        Logger::Log("here2");
        auto &aTransform = i->GetComponent<TransformComponent>();
        auto &aCollision = j->GetComponent<CollisionComponent>();
        auto &bTransform = i->GetComponent<TransformComponent>();
        auto &bCollision = j->GetComponent<CollisionComponent>();

        bool colliding = checkAABBCollision(
            aTransform.position.x + aCollision.offset.x,
            aTransform.position.y + aCollision.offset.y, aCollision.width,
            aCollision.height, bTransform.position.x + bCollision.offset.x,
            bTransform.position.y + bCollision.offset.y, bCollision.width,
            bCollision.height);
        if (colliding) {
          aCollision.isColliding = true;
          bCollision.isColliding = true;
        }
      }
    }
  }
};

#endif