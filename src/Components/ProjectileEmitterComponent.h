#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

struct ProjectileEmitterComponent {
  int projectileLifecycleDuration;
  glm::vec2 projectileVelocity;
  int frequency; // milliseconds
  int hitPercentDamage;
  Uint32 lastEmitTime; // milliseconds

  ProjectileEmitterComponent(int projectileLifecycleDuration = 1000,
                             glm::vec2 velocity = glm::vec2(20, 20),
                             int frequency = 1000, int hitPercentDamage = 10)
      : projectileLifecycleDuration(projectileLifecycleDuration),
        projectileVelocity(velocity), frequency(frequency),
        hitPercentDamage(hitPercentDamage), lastEmitTime(SDL_GetTicks()) {}
};