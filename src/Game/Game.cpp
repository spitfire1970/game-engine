#include "Game.h"
#include "../AssetStore/AssetStore.h"
#include "../Components/AnimationComponent.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/CollisionComponent.h"
#include "../Components/KeyboardControlComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TextLabelComponent.h"
#include "../Components/TransformComponent.h"
#include "../EventBus/EventBus.h"
#include "../Logger/Logger.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/CameraFollowSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Systems/DamageSystem.h"
#include "../Systems/KeyboardControlSystem.h"
#include "../Systems/KeyboardMovementSystem.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/ProjectileEmitterSystem.h"
#include "../Systems/ProjectileLifecycleSystem.h"
#include "../Systems/RenderCollisionSystem.h"
#include "../Systems/RenderGUISystem.h"
#include "../Systems/RenderHealthSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/RenderTextSystem.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <fstream>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdlrenderer2.h>
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

bool Game::Initialize() {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    Logger::Err("Error init sdl");
    return 1;
  }
  if (TTF_Init() != 0) {
    Logger::Err("Error init sdl ttf");
    return 1;
  }
  SDL_DisplayMode displayMode;
  SDL_GetCurrentDisplayMode(0, &displayMode);
  windowHeight = displayMode.h / 2;
  windowWidth = static_cast<int>(windowHeight * displayMode.w / displayMode.h);
  window =
      SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       displayMode.w, displayMode.h, SDL_WINDOW_BORDERLESS);
  if (!window) {
    Logger::Err("error creating window");
    return 1;
  }
  renderer =
      SDL_CreateRenderer(window, -1,
                         SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC |
                             SDL_RENDERER_TARGETTEXTURE);
  if (!renderer) {
    Logger::Err("Error creating renderer");
    return 1;
  }
  SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "nearest");
  // summary: game distances/calculations (i.e. "logical untis") are in the same
  // realm as the "window" (windowWidth/windowHeight) as well as game texture
  // (sized to windowWidth/windowHeight).
  // they are exactly the same ( by exactly equal i mean that it's not just same
  // "realm", it's that the values is the exact same in window dimensions as in
  // our logical units) for fixed HUD entities and are the exact same after
  // subtracting the camera offset from the other game values.
  // the windowWidth/windowHeight are also the same as cameraWidth/cameraHeight
  // implicitly (i.e. they are the same thing in our case since our entire
  // window is just the game camera) so a better name is probably
  // viewWidth/viewHeight.

  // the mapWidth/mapHeight are calculated based solely on tileScale and the
  // number and size of individual tiles (could be smaller or larger than
  // texture/window which determines the amount of "zoom" of the map relative to
  // the screen)

  // we logically treat the above two together because they are related (like
  // using the map dimensions for boundaries)

  // the display is in a separate realm of units, but we make sure that our
  // window/game as in the same ratio and then SDL takes care of
  // expanding/contracting the game texture to the display when we call
  // SDL_RenderCopy
  gameTarget =
      SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
                        SDL_TEXTUREACCESS_TARGET, windowWidth, windowHeight);
  SDL_RendererInfo info;
  SDL_GetRendererInfo(renderer, &info);
  Logger::Log(std::string("video: ") + SDL_GetCurrentVideoDriver() +
              ", renderer: " + info.name);

  // Initialize the ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  (void)io;
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  io.ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls
  ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
  ImGui_ImplSDLRenderer2_Init(renderer);

  isRunning = true;
  SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
  return 0;
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
    ImGui_ImplSDL2_ProcessEvent(&sdlEvent);

    switch (sdlEvent.type) {
    case SDL_QUIT:
      isRunning = false;
      break;
    case SDL_KEYDOWN:
      eventBus->emitEvent<KeyboardEvent>(sdlEvent.key.keysym.sym);
      if (sdlEvent.key.keysym.sym == SDLK_ESCAPE)
        isRunning = false;

      if (sdlEvent.key.keysym.sym == SDLK_d)
        debugMode = !debugMode;

      break;
    }
  }
}

void Game::LoadLevel(int level) {
  registry->AddSystem<MovementSystem>();
  registry->AddSystem<RenderSystem>();
  registry->AddSystem<CollisionSystem>();
  registry->AddSystem<RenderCollisionSystem>();
  registry->AddSystem<AnimationSystem>();
  registry->AddSystem<DamageSystem>();
  registry->AddSystem<KeyboardMovementSystem>();
  registry->AddSystem<KeyboardControlSystem>();
  registry->AddSystem<CameraFollowSystem>();
  registry->AddSystem<ProjectileEmitterSystem>();
  registry->AddSystem<ProjectileLifecycleSystem>();
  registry->AddSystem<RenderTextSystem>();
  registry->AddSystem<RenderHealthSystem>();
  registry->AddSystem<RenderGUISystem>();

  assetStore->AddTexture(renderer, "tank-image",
                         "./assets/images/tank-panther-right.png");
  assetStore->AddTexture(renderer, "truck-image",
                         "./assets/images/truck-ford-right.png");
  assetStore->AddTexture(renderer, "tree-image", "./assets/images/tree.png");
  assetStore->AddTexture(renderer, "tilemap-image",
                         "./assets/tilemaps/jungle.png");
  assetStore->AddTexture(renderer, "chopper-image",
                         "./assets/images/chopper-spritesheet.png");
  assetStore->AddTexture(renderer, "radar-image", "./assets/images/radar.png");
  assetStore->AddTexture(renderer, "bullet-image",
                         "./assets/images/bullet.png");
  assetStore->AddFont(20, "charriot-font-20", "./assets/fonts/charriot.ttf");
  assetStore->AddFont(8, "arial-font-8", "./assets/fonts/arial.ttf");

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
                                         srcRectX, srcRectY);
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
      5000, glm::vec2(150.0), std::numeric_limits<int>::max());
  chopper.AddComponent<CameraFollowComponent>();
  chopper.AddComponent<TextLabelComponent>();

  Entity chopper2 = registry->CreateEntity();
  chopper2.AddComponent<TransformComponent>(glm::vec2(500.0, 200.0),
                                            glm::vec2(1.0, 1.0), 0.0);
  chopper2.AddComponent<RigidBodyComponent>(glm::vec2(-20.0, -20.0));
  chopper2.AddComponent<SpriteComponent>("chopper-image", 32, 32, 2);
  chopper2.AddComponent<AnimationComponent>(2, 10);
  chopper2.AddComponent<CollisionComponent>(32, 32);
  chopper2.AddComponent<HealthComponent>(100);

  chopper2.AddComponent<KeyboardControlComponent>(SDLK_w, SDLK_d, SDLK_s,
                                                  SDLK_a, 100.0);

  Entity tank = registry->CreateEntity();
  tank.Group("enemies");
  tank.AddComponent<TransformComponent>(glm::vec2(500.0, 200.0),
                                        glm::vec2(1.0, 1.0), 0.0);
  tank.AddComponent<RigidBodyComponent>(glm::vec2(20.0, 0.0));
  tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 2);
  tank.AddComponent<CollisionComponent>(32, 32);
  tank.AddComponent<HealthComponent>(100);
  tank.AddComponent<TextLabelComponent>();

  Entity truck = registry->CreateEntity();
  truck.Group("enemies");
  truck.AddComponent<TransformComponent>(glm::vec2(100.0, 300.0),
                                         glm::vec2(1.0, 1.0), 90.0);
  truck.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 25.0));
  truck.AddComponent<HealthComponent>(100);
  truck.AddComponent<TextLabelComponent>();

  truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 1);
  truck.AddComponent<CollisionComponent>(32, 32);
  truck.AddComponent<ProjectileEmitterComponent>(5000, glm::vec2(0.0, 100.0),
                                                 2000);

  Entity treeA = registry->CreateEntity();
  treeA.Group("obstacles");
  treeA.AddComponent<TransformComponent>(glm::vec2(400.0, 200.0),
                                         glm::vec2(1.0, 1.0), 0.0);
  treeA.AddComponent<SpriteComponent>("tree-image", 16, 32, 1);
  treeA.AddComponent<CollisionComponent>(14, 32);

  Entity treeB = registry->CreateEntity();
  treeB.Group("obstacles");
  treeB.AddComponent<TransformComponent>(glm::vec2(600.0, 200.0),
                                         glm::vec2(1.0, 1.0), 0.0);
  treeB.AddComponent<SpriteComponent>("tree-image", 16, 32, 1);
  treeB.AddComponent<CollisionComponent>(14, 32);

  Entity treeC = registry->CreateEntity();
  treeC.Group("obstacles");
  treeC.AddComponent<TransformComponent>(glm::vec2(100.0, 200.0),
                                         glm::vec2(1.0, 1.0), 0.0);
  treeC.AddComponent<SpriteComponent>("tree-image", 16, 32, 1);
  treeC.AddComponent<CollisionComponent>(14, 32);

  Entity treeD = registry->CreateEntity();
  treeD.Group("obstacles");
  treeD.AddComponent<TransformComponent>(glm::vec2(100.0, 400.0),
                                         glm::vec2(1.0, 1.0), 0.0);
  treeD.AddComponent<SpriteComponent>("tree-image", 16, 32, 1);
  treeD.AddComponent<CollisionComponent>(14, 32);

  Entity radar = registry->CreateEntity();
  radar.AddComponent<TransformComponent>(glm::vec2(windowWidth - 74.0, 10.0),
                                         glm::vec2(1.0, 1.0), 0.0, true);
  radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 1);
  radar.AddComponent<AnimationComponent>(8, 5);

  Entity label = registry->CreateEntity();
  SDL_Color green = {0, 255, 0};
  label.AddComponent<TransformComponent>(glm::vec2(windowWidth / 2 - 40, 10),
                                         true);
  label.AddComponent<TextLabelComponent>("CHOPPER 1.0", "charriot-font-20",
                                         green);
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
  registry->GetSystem<MovementSystem>().SubscribeToEvents(*eventBus);
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
  // do things in backbuffer
  // 1. Draw the game into the small off-screen texture
  SDL_SetRenderTarget(renderer, gameTarget);
  SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);

  // clear backbuffer with dark grey
  SDL_RenderClear(renderer);
  registry->GetSystem<RenderSystem>().Update(renderer, *assetStore, camera);
  registry->GetSystem<RenderTextSystem>().Update(renderer, *assetStore, camera);
  registry->GetSystem<RenderHealthSystem>().Update(renderer, camera);
  if (debugMode) {
    registry->GetSystem<RenderCollisionSystem>().Update(renderer, camera);
  }

  // 2. Switch back to the window  (passing nullptr uses the window the renderer
  // was created with)
  SDL_SetRenderTarget(renderer, nullptr);

  // clear window with black
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);

  // and stretch the game to fill it
  SDL_RenderCopy(renderer, gameTarget, nullptr, nullptr);

  // 3. ImGui on top, at native resolution
  if (debugMode) {
    registry->GetSystem<RenderGUISystem>().Update(renderer, registry, camera);
  }
  // replace front buffer with back buffer
  SDL_RenderPresent(renderer);
}

void Game::Destroy() {
  ImGui_ImplSDLRenderer2_Shutdown();
  ImGui_ImplSDL2_Shutdown();
  ImGui::DestroyContext();
  assetStore->ClearAssets();
  SDL_DestroyTexture(gameTarget);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  TTF_Quit();
  SDL_Quit();
}
