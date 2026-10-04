#pragma once

#include <SDL2/SDL.h>

struct ProjectileComponent {
  int duration;
  int startTime;

  ProjectileComponent(int duration = 0) {
    this->duration = duration;
    this->startTime = SDL_GetTicks();
  }
};