#ifndef GAME_H
#define GAME_H
#include "../AssetStore/AssetStore.h"
#include "../ECS/ECS.h"
#include <SDL2/SDL.h>
#include <memory>

const int FPS = 1200;
const int MILLISECS_PER_FRAME = 1000 / FPS;

class Game {
private:
  int millisecsPrevFrame = 0;
  bool isRunning;
  SDL_Window *window;
  SDL_Renderer *renderer;
  std::unique_ptr<Registry> registry;
  std::unique_ptr<AssetStore> assetStore;
  bool debugMode = false;

public:
  Game();
  ~Game();
  void Initialize();
  void Run();
  void LoadLevel(int level);
  void Setup();
  void ProcessInput();
  void Update();
  void Render();
  void Destroy();

  int windowWidth;
  int windowHeight;
};

#endif
