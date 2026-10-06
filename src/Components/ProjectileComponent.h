#ifndef PROJECTILECOMPONENT_H
#define PROJECTILECOMPONENT_H

#include <SDL2/SDL.h>

struct ProjectileComponent {
  bool isFriendly;
  int hitPercentDamage;
  int lifecycleDuration;
  int startTime;

  ProjectileComponent(bool isFriendly = false, int hitPercentDamage = 0,
                      int lifecycleDuration = 0) {
    this->isFriendly = isFriendly;
    this->hitPercentDamage = hitPercentDamage;
    this->lifecycleDuration = lifecycleDuration;
    this->startTime = SDL_GetTicks();
  }
};

#endif
