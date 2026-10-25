#ifndef SPRITECOMPONENT_H
#define SPRITECOMPONENT_H

#include <string>

struct SpriteComponent {
  std::string assetId;
  int width;
  int height;
  int zIndex;
  SDL_Rect srcRect;
  SDL_RendererFlip flip;

  SpriteComponent(std::string assetId = "", int width = 0, int height = 0,
                  int zIndex = 0, int srcRectX = 0, int srcRectY = 0)
      : assetId(assetId), width(width), height(height), zIndex(zIndex) {
    this->srcRect = {srcRectX, srcRectY, width, height};
    flip = SDL_FLIP_NONE;
  }
};

#endif
