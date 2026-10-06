#include "Game.h"
#include "../AssetStore/AssetStore.h"
#include "../Components/AnimationComponent.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/CollisionComponent.h"
#include "../Components/KeyboardControlComponent.h"
#include "../Components/ProjectileEmitterComponent.h"

#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../EventBus/EventBus.h"
#include "../Logger/Logger.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/CameraFollowSystem.h"
#include "../Systems/CollisionRenderSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Systems/DamageSystem.h"
#include "../Systems/KeyboardControlSystem.h"
#include "../Systems/KeyboardMovementSystem.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/ProjectileEmitterSystem.h"
#include "../Systems/ProjectileLifecycleSystem.h"
#include "../Systems/RenderSystem.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <limits>
#include <memory>

int Game::windowWidth;
int Game::windowHeight;
int Game::mapWidth;
int Game::mapHeight;

Game::Game() {
  isRunning = false;
  registry = std::make_unique<Registry>();
  assetStore = std::make_unique<AssetStore>();
  eventBus = std::make_unique<EventBus>();
  Logger::Log("Game constructor called!");
}

Game::~Game() { Logger::Log("Game destructor called!"); }

void Game::Initialize() {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    Logger::Err("Error init sdl");
    return;
  }
  SDL_DisplayMode displayMode;
  SDL_GetCurrentDisplayMode(0, &displayMode);
  windowWidth = 800;
  windowHeight = 600;
  window =
      SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       windowWidth, windowHeight, SDL_WINDOW_BORDERLESS);
  if (!window) {
    Logger::Err("error creating window");
    return;
  }
  renderer = SDL_CreateRenderer(
      window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!renderer) {
    Logger::Err("Error creating renderer");
    return;
  }
  isRunning = true;
  SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
}

void Game::Run() {
  Setup();
  while (isRunning) {
    ProcessInput();
    Update();
    Render();
  }
}

void Game::ProcessInput() {
  SDL_Event sdlEvent;
  while (SDL_PollEvent(&sdlEvent)) {
    switch (sdlEvent.type) {
    case SDL_QUIT:
      isRunning = false;
      break;
    case SDL_KEYDOWN:
      eventBus->emitEvent<KeyboardEvent>(sdlEvent.key.keysym.sym);
      if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
        isRunning = false;
      }
      if (sdlEvent.key.keysym.sym == SDLK_d) {
        debugMode = !debugMode;
      }
      break;
    }
  }
}

void Game::LoadLevel(int level) {
  registry->AddSystem<MovementSystem>();
  registry->AddSystem<RenderSystem>();
  registry->AddSystem<CollisionSystem>();
  registry->AddSystem<CollisionRenderSystem>();
  registry->AddSystem<AnimationSystem>();
  registry->AddSystem<DamageSystem>();
  registry->AddSystem<KeyboardMovementSystem>();
  registry->AddSystem<KeyboardControlSystem>();
  registry->AddSystem<CameraFollowSystem>();
  registry->AddSystem<ProjectileEmitterSystem>();
  registry->AddSystem<ProjectileLifecycleSystem>();

  assetStore->AddTexture(renderer, "tank-image",
                         "./assets/images/tank-panther-right.png");
  assetStore->AddTexture(renderer, "truck-image",
                         "./assets/images/truck-ford-right.png");
  assetStore->AddTexture(renderer, "tilemap-image",
                         "./assets/tilemaps/jungle.png");
  assetStore->AddTexture(renderer, "chopper-image",
                         "./assets/images/chopper-spritesheet.png");
  assetStore->AddTexture(renderer, "radar-image", "./assets/images/radar.png");
  assetStore->AddTexture(renderer, "bullet-image",
                         "./assets/images/bullet.png");

  // Load the tilemap
  int tileSize = 32;
  double tileScale = 1.5;
  int mapNumCols = 25;
  int mapNumRows = 20;
  Game::mapWidth = tileSize * mapNumCols * tileScale;
  Game::mapHeight = tileSize * mapNumRows * tileScale;

  std::fstream mapFile;
  mapFile.open("./assets/tilemaps/jungle.map");

  for (int y = 0; y < mapNumRows; y++) {
    for (int x = 0; x < mapNumCols; x++) {
      char ch;
      mapFile.get(ch);
      int srcRectY = std::atoi(&ch) * tileSize;
      mapFile.get(ch);
      int srcRectX = std::atoi(&ch) * tileSize;
      mapFile.ignore();

      Entity tile = registry->CreateEntity();
      tile.Group("tiles");
      tile.AddComponent<TransformComponent>(
          glm::vec2(x * (tileScale * tileSize), y * (tileScale * tileSize)),
          glm::vec2(tileScale, tileScale), 0.0);
      tile.AddComponent<SpriteComponent>("tilemap-image", tileSize, tileSize, 0,
                                         false, srcRectX, srcRectY);
    }
  }
  mapFile.close();
  Entity chopper = registry->CreateEntity();
  chopper.AddComponent<TransformComponent>(glm::vec2(100.0, 200.0),
                                           glm::vec2(1.0, 1.0), 0.0);
  chopper.Tag("player");
  chopper.AddComponent<RigidBodyComponent>(glm::vec2(20.0, 20.0));
  chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 2);
  chopper.AddComponent<AnimationComponent>(2, 10);
  chopper.AddComponent<CollisionComponent>(32, 32);
  chopper.AddComponent<HealthComponent>(100);
  chopper.AddComponent<KeyboardControlComponent>(SDLK_UP, SDLK_RIGHT, SDLK_DOWN,
                                                 SDLK_LEFT, 100.0);
  chopper.AddComponent<ProjectileEmitterComponent>(
      5000, std::numeric_limits<int>::max(), 150.0);
  chopper.AddComponent<CameraFollowComponent>();

  Entity chopper2 = registry->CreateEntity();
  chopper2.AddComponent<TransformComponent>(glm::vec2(500.0, 200.0),
                                            glm::vec2(1.0, 1.0), 0.0);
  chopper2.AddComponent<RigidBodyComponent>(glm::vec2(20.0, 20.0));
  chopper2.AddComponent<SpriteComponent>("chopper-image", 32, 32, 2);
  chopper2.AddComponent<AnimationComponent>(2, 10);
  chopper2.AddComponent<CollisionComponent>(32, 32);
  chopper2.AddComponent<HealthComponent>(100);

  chopper2.AddComponent<KeyboardControlComponent>(SDLK_w, SDLK_d, SDLK_s,
                                                  SDLK_a, 100.0);

  Entity tank = registry->CreateEntity();
  tank.Group("enemies");
  tank.AddComponent<TransformComponent>(glm::vec2(10.0, 30.0),
                                        glm::vec2(1.0, 1.0), 0.0);
  tank.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
  tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 2);
  tank.AddComponent<CollisionComponent>(32, 32);
  tank.AddComponent<HealthComponent>(100);
  tank.AddComponent<ProjectileEmitterComponent>(3000, 1000,
                                                glm::vec2(50.0, 0.0));

  Entity truck = registry->CreateEntity();
  truck.Group("enemies");
  truck.AddComponent<TransformComponent>(glm::vec2(100.0, 30.0),
                                         glm::vec2(1.0, 1.0), 0.0);
  truck.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
  truck.AddComponent<HealthComponent>(100);

  truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 1);
  truck.AddComponent<CollisionComponent>(32, 32);
  truck.AddComponent<ProjectileEmitterComponent>(5000, 2000,
                                                 glm::vec2(0.0, 100.0));

  Entity radar = registry->CreateEntity();
  radar.AddComponent<TransformComponent>(glm::vec2(windowWidth - 74.0, 10.0),
                                         glm::vec2(1.0, 1.0), 0.0);
  radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 1, true);
  radar.AddComponent<AnimationComponent>(8, 5);
}

void Game::Setup() { LoadLevel(1); }
void Game::Update() {
  // a tick is 1 millisecond
  // while (SDL_GetTicks() < millisecsPrevFrame + MILLISECS_PER_FRAME);

  int timeToWait = MILLISECS_PER_FRAME + millisecsPrevFrame - SDL_GetTicks();
  assert(timeToWait <= MILLISECS_PER_FRAME);
  if (timeToWait > 0)
    SDL_Delay(timeToWait);

  double deltaTime = (SDL_GetTicks() - millisecsPrevFrame) / 1000.0;

  millisecsPrevFrame = SDL_GetTicks();

  eventBus->Reset();

  // Perform the subscription of the events for all systems
  registry->GetSystem<DamageSystem>().SubscribeToEvents(*eventBus);
  registry->GetSystem<KeyboardMovementSystem>().SubscribeToEvents(*eventBus);
  registry->GetSystem<KeyboardControlSystem>().SubscribeToEvents(*eventBus);
  registry->GetSystem<ProjectileEmitterSystem>().SubscribeToEvents(*eventBus);
  registry->Update();

  // to make velocity act with respect to time rather than frame rate
  registry->GetSystem<MovementSystem>().Update(deltaTime);
  registry->GetSystem<AnimationSystem>().Update();
  registry->GetSystem<CameraFollowSystem>().Update(camera);
  registry->GetSystem<CollisionSystem>().Update(*eventBus);
  registry->GetSystem<ProjectileEmitterSystem>().Update();
  registry->GetSystem<ProjectileLifecycleSystem>().Update();
}

void Game::Render() {

  // do things in back buffer
  SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
  SDL_RenderClear(renderer);
  registry->GetSystem<RenderSystem>().Update(renderer, *assetStore, camera);
  if (debugMode) {
    registry->GetSystem<CollisionRenderSystem>().Update(renderer, camera);
  }
  // replace front buffer with back buffer
  SDL_RenderPresent(renderer);
}

void Game::Destroy() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
