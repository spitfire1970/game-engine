#include "Game.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TransformComponent.h"
#include "../Logger/Logger.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>

Game::Game() {
  isRunning = false;
  registry = std::make_unique<Registry>();
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
      if (sdlEvent.key.keysym.sym == SDLK_ESCAPE) {
        isRunning = false;
      }
      break;
    }
  }
}

glm::vec2 playerPosition;
glm::vec2 playerVelocity;

void Game::Setup() {
  registry->AddSystem<MovementSystem>();
  registry->AddSystem<RenderSystem>();
  Entity tank = registry->CreateEntity();

  tank.AddComponent<TransformComponent>(glm::vec2(10.0, 30.0),
                                        glm::vec2(1.0, 1.0), 0.0);
  tank.AddComponent<RigidBodyComponent>(glm::vec2(30.0, 50.0));
  tank.AddComponent<SpriteComponent>(10, 10);

  Entity truck = registry->CreateEntity();
  truck.AddComponent<TransformComponent>(glm::vec2(10.0, 30.0),
                                         glm::vec2(1.0, 1.0), 0.0);
  truck.AddComponent<RigidBodyComponent>(glm::vec2(40.0, 20.0));
  truck.AddComponent<SpriteComponent>(10, 50);
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
  // to make velocity act with respect to time rather than frame rate
  registry->GetSystem<MovementSystem>().Update(deltaTime);
  registry->Update();
}

void Game::Render() {

  // do things in back buffer
  SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
  SDL_RenderClear(renderer);
  registry->GetSystem<RenderSystem>().Update(renderer);
  // replace front buffer with back buffer
  SDL_RenderPresent(renderer);
}

void Game::Destroy() {
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
