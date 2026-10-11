#include "AssetStore.h"
#include "../Logger/Logger.h"
#include "SDL2/SDL_image.h"

AssetStore::~AssetStore() {
  Logger::Log("Calling AssetStore destructor");
  ClearAssets();
}

void AssetStore::ClearAssets() {
  for (auto texture : textures) {
    SDL_DestroyTexture(texture.second);
  }
  textures.clear();
  for (auto font : fonts) {
    TTF_CloseFont(font.second);
  }
  fonts.clear();
};
void AssetStore::AddTexture(SDL_Renderer *renderer, const std::string &assetId,
                            const std::string &filePath) {
  SDL_Surface *surface = IMG_Load(filePath.c_str());
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  SDL_FreeSurface(surface);

  textures.emplace(assetId, texture);
};
SDL_Texture *AssetStore::GetTexture(const std::string &assetId) {
  return textures[assetId];
};

void AssetStore::AddFont(int fontSize, const std::string &assetId,
                         const std::string &filePath) {
  TTF_Font *font = TTF_OpenFont(filePath.c_str(), fontSize);
  fonts.emplace(assetId, font);
};

TTF_Font *AssetStore::GetFont(const std::string &assetId) {
  return fonts[assetId];
};