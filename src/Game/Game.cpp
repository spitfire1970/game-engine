#include "Game.h"
#include "../AssetStore/AssetStore.h"
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
#include "LevelLoader.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <fstream>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl2.h>
#include <imgui/imgui_impl_sdlrenderer2.h>
#include <iostream>
#include <memory>

int Game::windowWidth;
int Game::windowHeight;
int Game::mapWidth;
int Game::mapHeight;
sol::state Game::lua;

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

void Game::Setup() {
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

  lua.open_libraries(sol::lib::base, sol::lib::math);
  LevelLoader levelLoader(*assetStore, renderer, *registry);
  levelLoader.LoadLevel(1);
}
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
