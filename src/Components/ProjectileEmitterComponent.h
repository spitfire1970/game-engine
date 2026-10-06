#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

struct ProjectileEmitterComponent {
  int projectileLifecycleDuration;
  int frequency;       // milliseconds
  Uint32 lastEmitTime; // milliseconds
  float speed;
  glm::vec2 projectileVelocity;
  int hitPercentDamage;

  ProjectileEmitterComponent(int projectileLifecycleDuration = 1000,
                             int frequency = 1000, float speed = 15.0f,
                             int hitPercentDamage = 10)
      : projectileLifecycleDuration(projectileLifecycleDuration),
        frequency(frequency), lastEmitTime(SDL_GetTicks()), speed(speed),
        projectileVelocity(speed, speed), hitPercentDamage(hitPercentDamage) {}

  ProjectileEmitterComponent(int projectileLifecycleDuration, int frequency,
                             glm::vec2 velocity, int hitPercentDamage = 10)
      : projectileLifecycleDuration(projectileLifecycleDuration),
        frequency(frequency), lastEmitTime(SDL_GetTicks()),
        speed(glm::length(velocity)), projectileVelocity(velocity),
        hitPercentDamage(hitPercentDamage) {}
};