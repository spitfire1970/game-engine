#pragma once

struct KeyboardControlComponent {
  SDL_Keycode upKey;
  SDL_Keycode rightKey;
  SDL_Keycode downKey;
  SDL_Keycode leftKey;
  float speed;

  KeyboardControlComponent(SDL_Keycode upKey = SDLK_UNKNOWN,
                           SDL_Keycode rightKey = SDLK_UNKNOWN,
                           SDL_Keycode downKey = SDLK_UNKNOWN,
                           SDL_Keycode leftKey = SDLK_UNKNOWN,
                           float speed = 0.0)
      : upKey(upKey), rightKey(rightKey), downKey(downKey), leftKey(leftKey),
        speed(speed) {}
};