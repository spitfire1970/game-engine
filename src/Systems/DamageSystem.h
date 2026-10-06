#ifndef DAMAGESYSTEM_H
#define DAMAGESYSTEM_H

#include "../Components/CollisionComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"

class DamageSystem : public System {
public:
  DamageSystem() { RequireComponent<CollisionComponent>(); }

  void SubscribeToEvents(EventBus &eventBus) {
    auto f = [this](CollisionEvent &event) { OnCollision(event); };
    eventBus.subscribe<CollisionEvent>(f);
  }

  void OnCollision(CollisionEvent &event) {
    Entity a = event.a;
    Entity b = event.b;
    Logger::Log("Collision event emitted: " + std::to_string(a.GetId()) +
                " and " + std::to_string(b.GetId()));

    if (a.BelongsToGroup("projectiles") && b.HasTag("player")) {
      OnProjectileHitsEntity(a, b, true);
    }

    if (b.BelongsToGroup("projectiles") && a.HasTag("player")) {
      OnProjectileHitsEntity(b, a, true);
    }

    if (a.BelongsToGroup("projectiles") && b.BelongsToGroup("enemies")) {
      OnProjectileHitsEntity(a, b, false);
    }

    if (b.BelongsToGroup("projectiles") && a.BelongsToGroup("enemies")) {
      OnProjectileHitsEntity(b, a, false);
    }
  }

  void OnProjectileHitsEntity(Entity projectile, Entity entity,
                              bool entityFriendliness) {
    const auto &projectileComponent =
        projectile.GetComponent<ProjectileComponent>();

    if (projectileComponent.isFriendly != entityFriendliness) {
      // Reduce the health of the player by the projectile hitPercentDamage
      auto &health = entity.GetComponent<HealthComponent>();
      // Subtract the health of the player
      health.healthPercentage -= projectileComponent.hitPercentDamage;
      // Kills the player when health reaches zero
      if (health.healthPercentage <= 0) {
        entity.Kill();
      }

      // Kill the projectile
      projectile.Kill();
    }
  }
};

#endif
