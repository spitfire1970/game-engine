#ifndef ASSETSTORE_H
#define ASSETSTORE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <map>
#include <string>

class AssetStore {
private:
  std::map<std::string, SDL_Texture *> textures;
  std::map<std::string, TTF_Font *> fonts;

public:
  AssetStore() = default;
  ~AssetStore();

  void ClearAssets();
  void AddTexture(SDL_Renderer *renderer, const std::string &assetId,
                  const std::string &filePath);
  SDL_Texture *GetTexture(const std::string &assetId);
  void AddFont(int fontSize, const std::string &assetId,
               const std::string &filePath);
  TTF_Font *GetFont(const std::string &assetId);
};

#endif