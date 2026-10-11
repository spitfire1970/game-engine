#pragma once

#include "../AssetStore/AssetStore.h"
#include "../Components/TextLabelComponent.h"
#include "../ECS/ECS.h"
#include <SDL2/SDL.h>

class RenderTextSystem : public System {
public:
  RenderTextSystem() { RequireComponent<TextLabelComponent>(); }

  void Update(SDL_Renderer *renderer, AssetStore &assetStore,
              glm::vec2 &camera) {
    for (auto entity : GetSystemEntities()) {
      const auto &textlabel = entity.GetComponent<TextLabelComponent>();

      SDL_Surface *surface =
          TTF_RenderText_Blended(assetStore.GetFont(textlabel.assetId),
                                 textlabel.text.c_str(), textlabel.color);

      // could potentially cache this texture itself in the assetStore? for
      // texts with the exact same label, size, and colour
      SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
      SDL_FreeSurface(surface);

      int labelWidth = 0;
      int labelHeight = 0;

      // the fontSize is already baked into the texture that SDL creates with
      // the TTF_Font type so we can basically query/retrieve it
      SDL_QueryTexture(texture, NULL, NULL, &labelWidth, &labelHeight);

      SDL_Rect dstRect = {static_cast<int>(textlabel.position.x -
                                           (textlabel.isFixed ? 0 : camera.x)),
                          static_cast<int>(textlabel.position.y -
                                           (textlabel.isFixed ? 0 : camera.y)),
                          labelWidth, labelHeight};

      // srcRect is NULL because we want the entire texture to be mapped to
      // dstRect
      SDL_RenderCopy(renderer, texture, NULL, &dstRect);

      SDL_DestroyTexture(texture);
    }
  }
};