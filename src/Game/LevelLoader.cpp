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
#include <limits>
#include <sol/sol.hpp>

LevelLoader::LevelLoader(AssetStore &assetStore, SDL_Renderer *renderer,
                         Registry &registry)
    : assetStore(assetStore), renderer(renderer), registry(registry) {}
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
  Game::lua["window_width"] = Game::windowWidth;
  // Executes the script using the Sol state
  Game::lua.script_file("./assets/scripts/Level" + std::to_string(levelNumber) +
                        ".lua");

  // Read the big table for the current level
  sol::table level = Game::lua["Level"];

  ////////////////////////////////////////////////////////////////////////////
  // Read the level assets
  ////////////////////////////////////////////////////////////////////////////
  sol::table assets = level["assets"];

  for (std::size_t i = 1; i <= assets.size(); i++) {
    sol::table asset = assets[i];
    std::string assetType = asset["type"].get<std::string>();
    std::string assetId = asset["id"].get<std::string>();
    std::string assetFile = asset["file"].get<std::string>();

    if (assetType == "texture") {
      assetStore.AddTexture(renderer, assetId, assetFile);
      Logger::Log("A new texture asset was added to the asset store, id: " +
                  assetId);
    } else if (assetType == "font") {
      assetStore.AddFont(asset["font_size"].get<int>(), assetId, assetFile);
      Logger::Log("A new font asset was added to the asset store, id: " +
                  assetId);
    }
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
      int srcRectY = (ch - '0') * tileSize;
      mapFile.get(ch);
      int srcRectX = (ch - '0') * tileSize;
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
  sol::table entities = level["entities"];
  for (std::size_t i = 1; i <= entities.size(); i++) {
    sol::table entity = entities[i];
    Entity newEntity = registry.CreateEntity();

    // Tag
    sol::optional<std::string> tag = entity["tag"];
    if (tag) {
      newEntity.Tag(*tag);
    }

    // Group
    sol::optional<std::string> group = entity["group"];
    if (group) {
      newEntity.Group(*group);
    }

    // Components
    sol::optional<sol::table> hasComponents = entity["components"];
    if (!hasComponents) {
      continue;
    }
    sol::table components = *hasComponents;

    // Transform
    sol::optional<sol::table> transform = components["transform"];
    if (transform) {
      sol::table t = *transform;
      newEntity.AddComponent<TransformComponent>(
          glm::vec2(t["position"]["x"].get<float>(),
                    t["position"]["y"].get<float>()),
          glm::vec2(t["scale"]["x"].get_or(1.0), t["scale"]["y"].get_or(1.0)),
          t["rotation"].get_or(0.0), t["fixed"].get_or(false));
    }

    // RigidBody
    sol::optional<sol::table> rigidbody = components["rigidbody"];
    if (rigidbody) {
      sol::table r = *rigidbody;
      newEntity.AddComponent<RigidBodyComponent>(glm::vec2(
          r["velocity"]["x"].get_or(0.0), r["velocity"]["y"].get_or(0.0)));
    }

    // Sprite
    sol::optional<sol::table> sprite = components["sprite"];
    if (sprite) {
      sol::table s = *sprite;
      newEntity.AddComponent<SpriteComponent>(
          s["texture_asset_id"].get<std::string>(), s["width"].get<int>(),
          s["height"].get<int>(), s["z_index"].get_or(1));
    }

    // Animation
    sol::optional<sol::table> animation = components["animation"];
    if (animation) {
      sol::table a = *animation;
      newEntity.AddComponent<AnimationComponent>(a["num_frames"].get_or(1),
                                                 a["speed_rate"].get_or(1));
    }

    // Collision
    sol::optional<sol::table> collider = components["boxcollider"];
    if (collider) {
      sol::table c = *collider;
      newEntity.AddComponent<CollisionComponent>(
          c["width"].get<int>(), c["height"].get<int>(),
          glm::vec2(c["offset"]["x"].get_or(0.0),
                    c["offset"]["y"].get_or(0.0)));
    }

    // Health
    sol::optional<sol::table> health = components["health"];
    if (health) {
      sol::table h = *health;
      newEntity.AddComponent<HealthComponent>(
          static_cast<int>(h["health_percentage"].get_or(100)));
    }

    // KeyboardControl
    sol::optional<sol::table> keyboard = components["keyboard_controller"];
    if (keyboard) {
      sol::table k = *keyboard;
      auto key = [&k](const char *field, const char *fallback) {
        std::string name = k[field].get_or(std::string(fallback));
        return SDL_GetKeyFromName(name.c_str());
      };
      newEntity.AddComponent<KeyboardControlComponent>(
          key("up_key", "Up"), key("right_key", "Right"),
          key("down_key", "Down"), key("left_key", "Left"),
          k["speed"].get_or(100.0));
    }

    // ProjectileEmitter
    sol::optional<sol::table> projectileEmitter =
        components["projectile_emitter"];
    if (projectileEmitter) {
      sol::table p = *projectileEmitter;
      double durationSeconds = p["projectile_duration"].get_or(1.0);
      sol::optional<double> repeatSeconds = p["repeat_frequency"];
      int repeatMs = repeatSeconds
                         ? static_cast<int>(*repeatSeconds * 1000)
                         : std::numeric_limits<int>::max(); // no auto-fire
      newEntity.AddComponent<ProjectileEmitterComponent>(
          static_cast<int>(durationSeconds * 1000),
          glm::vec2(p["projectile_velocity"]["x"].get_or(20.0),
                    p["projectile_velocity"]["y"].get_or(20.0)),
          repeatMs, static_cast<int>(p["hit_percentage_damage"].get_or(10)));
    }

    // CameraFollow
    sol::optional<sol::table> cameraFollow = components["camera_follow"];
    if (cameraFollow) {
      newEntity.AddComponent<CameraFollowComponent>();
    }

    // TextLabel
    sol::optional<sol::table> textLabel = components["text_label"];
    if (textLabel) {
      sol::table l = *textLabel;
      sol::optional<std::string> text = l["text"];
      if (text) {
        SDL_Color color = {static_cast<Uint8>(l["color"]["r"].get_or(255)),
                           static_cast<Uint8>(l["color"]["g"].get_or(255)),
                           static_cast<Uint8>(l["color"]["b"].get_or(255)),
                           static_cast<Uint8>(l["color"]["a"].get_or(255))};
        newEntity.AddComponent<TextLabelComponent>(
            *text, l["font_asset_id"].get<std::string>(), color);
      } else {
        newEntity.AddComponent<TextLabelComponent>();
      }
    }
  }
}