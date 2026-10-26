#include "LevelLoader.h"
#include "../Components/AnimationComponent.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/CollisionComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/KeyboardControlComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/TextLabelComponent.h"
#include "../Components/TransformComponent.h"

#include "Game.h"
#include <fstream>
#include <sol/sol.hpp>

LevelLoader::LevelLoader(AssetStore &assetStore, SDL_Renderer *renderer,
                         Registry &registry)
    : assetStore(assetStore), renderer(renderer), registry(registry) {};
LevelLoader::~LevelLoader() = default;

void LevelLoader::LoadLevel(int levelNumber) {
  // This checks the syntax of our script, but it does not execute the script
  sol::load_result script = Game::lua.load_file(
      "./assets/scripts/Level" + std::to_string(levelNumber) + ".lua");
  if (!script.valid()) {
    sol::error err = script;
    std::string errorMessage = err.what();
    Logger::Err("Error loading the lua script: " + errorMessage);
    return;
  }

  // Executes the script using the Sol state
  Game::lua.script_file("./assets/scripts/Level" + std::to_string(levelNumber) +
                        ".lua");

  // Read the big table for the current level
  sol::table level = Game::lua["Level"];

  ////////////////////////////////////////////////////////////////////////////
  // Read the level assets
  ////////////////////////////////////////////////////////////////////////////
  sol::table assets = level["assets"];

  int i = 0;
  while (true) {
    sol::optional<sol::table> hasAsset = assets[i];
    if (hasAsset == sol::nullopt) {
      break;
    }
    sol::table asset = assets[i];
    std::string assetType = asset["type"];
    std::string assetId = asset["id"];
    if (assetType == "texture") {
      assetStore.AddTexture(renderer, assetId, asset["file"]);
      Logger::Log("A new texture asset was added to the asset store, id: " +
                  assetId);
    }
    if (assetType == "font") {
      assetStore.AddFont(asset["font_size"], assetId, asset["file"]);
      Logger::Log("A new font asset was added to the asset store, id: " +
                  assetId);
    }
    i++;
  }

  ////////////////////////////////////////////////////////////////////////////
  // Read the level tilemap information
  ////////////////////////////////////////////////////////////////////////////
  sol::table map = level["tilemap"];
  std::string mapFilePath = map["map_file"];
  std::string mapTextureAssetId = map["texture_asset_id"];
  int mapNumRows = map["num_rows"];
  int mapNumCols = map["num_cols"];
  int tileSize = map["tile_size"];
  double mapScale = map["scale"];
  std::fstream mapFile;
  mapFile.open(mapFilePath);
  for (int y = 0; y < mapNumRows; y++) {
    for (int x = 0; x < mapNumCols; x++) {
      char ch;
      mapFile.get(ch);
      int srcRectY = std::atoi(&ch) * tileSize;
      mapFile.get(ch);
      int srcRectX = std::atoi(&ch) * tileSize;
      mapFile.ignore();

      Entity tile = registry.CreateEntity();
      tile.AddComponent<TransformComponent>(
          glm::vec2(x * (mapScale * tileSize), y * (mapScale * tileSize)),
          glm::vec2(mapScale, mapScale), 0.0);
      tile.AddComponent<SpriteComponent>(mapTextureAssetId, tileSize, tileSize,
                                         0, srcRectX, srcRectY);
    }
  }
  mapFile.close();
  Game::mapWidth = mapNumCols * tileSize * mapScale;
  Game::mapHeight = mapNumRows * tileSize * mapScale;

  Entity chopper = registry.CreateEntity();
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

  Entity chopper2 = registry.CreateEntity();
  chopper2.AddComponent<TransformComponent>(glm::vec2(500.0, 200.0),
                                            glm::vec2(1.0, 1.0), 0.0);
  chopper2.AddComponent<RigidBodyComponent>(glm::vec2(-20.0, -20.0));
  chopper2.AddComponent<SpriteComponent>("chopper-image", 32, 32, 2);
  chopper2.AddComponent<AnimationComponent>(2, 10);
  chopper2.AddComponent<CollisionComponent>(32, 32);
  chopper2.AddComponent<HealthComponent>(100);

  chopper2.AddComponent<KeyboardControlComponent>(SDLK_w, SDLK_d, SDLK_s,
                                                  SDLK_a, 100.0);

  Entity tank = registry.CreateEntity();
  tank.Group("enemies");
  tank.AddComponent<TransformComponent>(glm::vec2(500.0, 200.0),
                                        glm::vec2(1.0, 1.0), 0.0);
  tank.AddComponent<RigidBodyComponent>(glm::vec2(20.0, 0.0));
  tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 2);
  tank.AddComponent<CollisionComponent>(32, 32);
  tank.AddComponent<HealthComponent>(100);
  tank.AddComponent<TextLabelComponent>();

  Entity truck = registry.CreateEntity();
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

  Entity treeA = registry.CreateEntity();
  treeA.Group("obstacles");
  treeA.AddComponent<TransformComponent>(glm::vec2(400.0, 200.0),
                                         glm::vec2(1.0, 1.0), 0.0);
  treeA.AddComponent<SpriteComponent>("tree-image", 16, 32, 1);
  treeA.AddComponent<CollisionComponent>(14, 32);

  Entity treeB = registry.CreateEntity();
  treeB.Group("obstacles");
  treeB.AddComponent<TransformComponent>(glm::vec2(600.0, 200.0),
                                         glm::vec2(1.0, 1.0), 0.0);
  treeB.AddComponent<SpriteComponent>("tree-image", 16, 32, 1);
  treeB.AddComponent<CollisionComponent>(14, 32);

  Entity treeC = registry.CreateEntity();
  treeC.Group("obstacles");
  treeC.AddComponent<TransformComponent>(glm::vec2(100.0, 200.0),
                                         glm::vec2(1.0, 1.0), 0.0);
  treeC.AddComponent<SpriteComponent>("tree-image", 16, 32, 1);
  treeC.AddComponent<CollisionComponent>(14, 32);

  Entity treeD = registry.CreateEntity();
  treeD.Group("obstacles");
  treeD.AddComponent<TransformComponent>(glm::vec2(100.0, 400.0),
                                         glm::vec2(1.0, 1.0), 0.0);
  treeD.AddComponent<SpriteComponent>("tree-image", 16, 32, 1);
  treeD.AddComponent<CollisionComponent>(14, 32);

  Entity radar = registry.CreateEntity();
  radar.AddComponent<TransformComponent>(
      glm::vec2(Game::windowWidth - 74.0, 10.0), glm::vec2(1.0, 1.0), 0.0,
      true);
  radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 1);
  radar.AddComponent<AnimationComponent>(8, 5);

  Entity label = registry.CreateEntity();
  SDL_Color green = {0, 255, 0};
  label.AddComponent<TransformComponent>(
      glm::vec2(Game::windowWidth / 2 - 40, 10), true);
  label.AddComponent<TextLabelComponent>("CHOPPER 1.0", "charriot-font-20",
                                         green);
};