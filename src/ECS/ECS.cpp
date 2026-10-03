#include "ECS.h"
#include "../Logger/Logger.h"

int IComponent::nextId = 0;
int Entity::GetId() const { return id; }
void Entity::Kill() { registry->KillEntity(*this); }

Entity Registry::CreateEntity() {
  int entityId;
  if (freeIds.size() > 0) {
    entityId = freeIds.front();
    freeIds.pop_front();
  } else {
    entityId = numEntities++;
    if (entityId >= entityComponentSignatures.size()) {
      entityComponentSignatures.resize(entityId + 1);
    }
  }
  Entity entity = Entity(entityId);
  entity.registry = this;
  entitiesToBeAdded.insert(entity);
  Logger::Log("New entity created with id = " + std::to_string(entityId));
  return entity;
}

void Registry::KillEntity(Entity entity) { entitiesToBeDeleted.insert(entity); }

void System::AddEntityToSystem(Entity entity) { entities.push_back(entity); }
void System::RemoveEntityFromSystem(Entity entity) {
  entities.erase(
      std::remove_if(entities.begin(), entities.end(),
                     [&entity](Entity other) { return entity == other; }),
      entities.end());
};

void Registry::AddEntityToSystems(Entity entity) {
  const auto entityId = entity.GetId();
  const auto entityComponentSignature = entityComponentSignatures[entityId];
  for (auto &systemItem : systems) {
    const auto &systemComponentSignature =
        systemItem.second->GetComponentSignature();
    bool isInterested = (entityComponentSignature & systemComponentSignature) ==
                        systemComponentSignature;
    if (isInterested) {
      systemItem.second->AddEntityToSystem(entity);
    }
  }
}

void Registry::RemoveEntityFromSystems(Entity entity) {
  for (auto &systemItem : systems) {
    systemItem.second->RemoveEntityFromSystem(entity);
  }
}

std::vector<Entity> &System::GetSystemEntities() { return entities; }

const Signature &System::GetComponentSignature() const {
  return componentSignature;
}

void Registry::Update() {
  for (auto entity : entitiesToBeDeleted) {
    RemoveEntityFromSystems(entity);
    entityComponentSignatures[entity.GetId()].reset();
    freeIds.push_back(entity.GetId());
  }
  entitiesToBeDeleted.clear();
  for (auto entity : entitiesToBeAdded) {
    AddEntityToSystems(entity);
  }
  entitiesToBeAdded.clear();
}