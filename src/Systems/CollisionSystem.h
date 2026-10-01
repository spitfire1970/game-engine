#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H

#include "../ECS/ECS.h"

class CollisionSystem {
public:
  CollisionSystem() {
    RequireComponent<TransformComponent>();
    RequireComponent<CollisionComponent>();
  }
  void Update() { for () }
};

#endif