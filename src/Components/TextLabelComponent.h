#pragma once

#include <SDL2/SDL.h>
#include <glm/glm.hpp>
#include <string>

struct TextLabelComponent {
  std::string text;
  std::string assetId;
  SDL_Color color;
  glm::vec2 offset;

  TextLabelComponent(const std::string &text = "",
                     const std::string &assetId = "",
                     const SDL_Color &color = {0, 0, 0},
                     glm::vec2 offset = glm::vec2(0))
      : text(text), assetId(assetId), color(color), offset(offset) {}
};
