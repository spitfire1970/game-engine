#ifndef KEYBOARDEVENT_H
#define KEYBOARDEVENT_H

#include "../EventBus/Event.h"
#include <SDL2/SDL.h>
#include <string>

struct KeyboardEvent : public Event {
  SDL_Keycode symbol;
  KeyboardEvent(SDL_Keycode symbol) : symbol(symbol) {};
};

#endif