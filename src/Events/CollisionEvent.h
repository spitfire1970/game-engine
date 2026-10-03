#ifndef COLLISIONEVENT_H
#define COLLISIONEVENT_H

#include "../ECS/ECS.h"
#include "../EventBus/Event.h"

struct CollisionEvent : public Event {
  Entity a;
  Entity b;
  CollisionEvent(Entity a, Entity b) : a(a), b(b) {}
};

#endif