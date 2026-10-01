#ifndef COLLISIONCOMPONENT_H
#define COLLISIONCOMPONENT_H

#include <glm/glm.hpp>

struct CollisionComponent {
  int width;
  int height;
  glm::vec2 offset;

  CollisionComponent(int width = 0, int height = 0,
                     glm::vec2 offset = glm::vec2(0))
      : width(width), height(height), offset(offset) {}
};

#endif