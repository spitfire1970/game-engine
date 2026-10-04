#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

struct ProjectileEmitterComponent {
  int projectileDuration;
  int frequency;       // milliseconds
  Uint32 lastEmitTime; // milliseconds
  float speed;
  glm::vec2 projectileVelocity;

  ProjectileEmitterComponent(int projectileDuration = 1000,
                             int frequency = 1000, float speed = 15.0f)
      : projectileDuration(projectileDuration), frequency(frequency),
        lastEmitTime(SDL_GetTicks()), speed(speed),
        projectileVelocity(speed, speed) {}

  ProjectileEmitterComponent(int projectileDuration, int frequency,
                             glm::vec2 velocity)
      : projectileDuration(projectileDuration), frequency(frequency),
        lastEmitTime(SDL_GetTicks()), speed(glm::length(velocity)),
        projectileVelocity(velocity) {}
};