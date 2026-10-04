#ifndef GAME_H
#define GAME_H
#include "../AssetStore/AssetStore.h"
#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include <SDL2/SDL.h>
#include <glm/glm.hpp>
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
  std::unique_ptr<EventBus> eventBus;
  bool debugMode = false;
  glm::vec2 camera = glm::vec2(0.0, 0.0);

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

  static int windowWidth;
  static int windowHeight;
  static int mapWidth;
  static int mapHeight;
};

#endif
