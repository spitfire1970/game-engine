#pragma once
#include "../AssetStore/AssetStore.h"
#include "Game.h"
#include <SDL2/SDL.h>

class LevelLoader {
private:
  AssetStore &assetStore;
  SDL_Renderer *renderer;
  Registry &registry;

public:
  LevelLoader(AssetStore &assetStore, SDL_Renderer *renderer,
              Registry &registry);
  ~LevelLoader();
  void LoadLevel(int level);
};