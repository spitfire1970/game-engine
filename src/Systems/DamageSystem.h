#ifndef DAMAGESYSTEM_H
#define DAMAGESYSTEM_H

#include "../Components/CollisionComponent.h"
#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"

class DamageSystem : public System {
public:
  DamageSystem() { RequireComponent<CollisionComponent>(); }

  void SubscribeToEvents(EventBus &eventBus) {

    auto f = [this](CollisionEvent &ev) { onCollision(ev); };
    eventBus.subscribe<CollisionEvent>(f);
  }

  void onCollision(CollisionEvent &ev) {
    ev.a.Kill();
    ev.b.Kill();
  }

  void Update() {}
};

#endif
