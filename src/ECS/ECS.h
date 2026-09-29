#ifndef ECS_H
#define ECS_H
#include "../Logger/Logger.h"
#include <bitset>
#include <set>
#include <typeindex>
#include <unordered_map>
#include <vector>

const unsigned int MAX_COMPONENTS = 32;

typedef std::bitset<MAX_COMPONENTS> Signature;
struct IComponent {
protected:
  static int nextId;
};

template <typename T> class Component : public IComponent {
public:
  static int GetId() { // static at function level just means this is a function
                       // at the class level
    static auto id = nextId++;
    // static inside a method means it's only evaluated once for that function
    // this fact combined with the fact that because of the templated class
    // there will exist many versions of this function (one per actual class),
    // it means each GetId for different classes is evaluated exactly once but
    // they all share the underlying nextId variable.
    return id;
  }
};

class Entity {
private:
  int id;

public:
  Entity(int id) : id(id) {};
  int GetId() const;
  bool operator==(const Entity &other) const { return id == other.id; }
  bool operator!=(const Entity &other) const { return id != other.id; }
  bool operator>(const Entity &other) const { return id > other.id; }
  bool operator<(const Entity &other) const { return id < other.id; }
  template <typename TComponent, typename... TArgs>
  void AddComponent(TArgs &&...args);
  template <typename TComponent> void RemoveComponent();
  template <typename TComponent> bool HasComponent() const;
  template <typename TComponent> TComponent &GetComponent() const;

  class Registry *registry;
};

class System {

private:
  Signature componentSignature;
  std::vector<Entity> entities;

public:
  System() = default;
  ~System() = default;
  void AddEntityToSystem(Entity entity);
  void RemoveEntityFromSystem(Entity entity);
  std::vector<Entity> GetSystemEntities() const;
  const Signature &GetComponentSignature() const;

  template <typename TComponent> void RequireComponent();
};

class IPool {
public:
  virtual ~IPool() = default;
};

template <typename T> class Pool : public IPool {
private:
  // index into this by entity id
  std::vector<T> data;

public:
  Pool(int size = 100) { Resize(size); }
  ~Pool() override = default;
  bool isEmpty() const { return data.empty(); }
  int GetSize() const { return data.size(); }
  void Resize(int n) { return data.resize(n); }
  void Clear() { return data.clear(); }
  void Add(T object) { data.push_back(object); }
  void Set(int index, T object) { data[index] = object; }
  T &Get(int index) { return static_cast<T &>(data[index]); }
  T &operator[](unsigned int index) { return data[index]; }
};

class Registry {
private:
  int numEntities = 0;
  std::set<Entity> entitiesToBeAdded;
  std::set<Entity> entitiesToBeDeleted;

  std::vector<std::shared_ptr<IPool>>
      componentPools; // index into this by component type id (not component id)
  std::vector<Signature>
      entityComponentSignatures; // index into this by entity id
  std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

public:
  Registry() = default;
  void Update();

  // Entity management
  Entity CreateEntity();
  void AddEntityToSystems(Entity entity);

  // Component management
  template <typename TComponent, typename... TArgs>
  void AddComponent(Entity entity, TArgs &&...args);
  template <typename TComponent> void RemoveComponent(Entity entity);
  template <typename TComponent> bool HasComponent(Entity entity) const;
  template <typename TComponent> TComponent &GetComponent(Entity entity) const;

  // System Management
  template <typename TSystem, typename... TArgs>
  void AddSystem(TArgs &&...args);
  template <typename TSystem> void RemoveSystem();
  template <typename TSystem> bool HasSystem() const;
  template <typename TSystem> TSystem &GetSystem() const;
};

template <typename TSystem, typename... TArgs>
void Registry::AddSystem(TArgs &&...args) {
  std::shared_ptr<TSystem> newSystem =
      std::make_shared<TSystem>(std::forward<TArgs>(args)...);
  systems.insert(std::pair(std::type_index(typeid(TSystem)), newSystem));
}

template <typename TSystem> void Registry::RemoveSystem() {
  auto system = systems.find(std::type_index(typeid(TSystem)));
  systems.erase(system);
}

template <typename TSystem> bool Registry::HasSystem() const {
  return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template <typename TSystem> TSystem &Registry::GetSystem() const {
  auto system = systems.find(std::type_index(typeid(TSystem)));
  return *std::static_pointer_cast<TSystem>(system->second);
}

template <typename TComponent> void System::RequireComponent() {
  const auto componentId = Component<TComponent>::GetId();
  componentSignature.set(componentId);
}

template <typename TComponent, typename... TArgs>
void Registry::AddComponent(Entity entity, TArgs &&...args) {
  const auto componentId = Component<TComponent>::GetId();
  const auto entityId = entity.GetId();

  if (componentId >= componentPools.size()) {
    componentPools.resize(componentId + 1, nullptr);
  }
  if (!componentPools[componentId]) {
    componentPools[componentId] = std::make_shared<Pool<TComponent>>();
  }
  std::shared_ptr<Pool<TComponent>> componentPool =
      std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);

  if (entityId >= componentPool->GetSize()) {
    componentPool->Resize(entityId + 1);
  }
  // can write this in direct initialization format
  TComponent newComponent = TComponent(std::forward<TArgs>(args)...);

  componentPool->Set(entityId, newComponent);

  entityComponentSignatures[entityId].set(componentId);

  Logger::Log("Component id " + std::to_string(componentId) +
              " was added to entity id " + std::to_string(entityId));
}

template <typename TComponent> void Registry::RemoveComponent(Entity entity) {
  const auto componentId = Component<TComponent>::GetId();
  const auto entityId = entity.GetId();

  entityComponentSignatures[entityId].set(componentId, false);
  Logger::Log("Component id " + std::to_string(componentId) +
              " was removed from entity id " + std::to_string(entityId));
}

template <typename TComponent>
bool Registry::HasComponent(Entity entity) const {
  const auto componentId = Component<TComponent>::GetId();
  const auto entityId = entity.GetId();

  return entityComponentSignatures[entityId].test(componentId);
}

template <typename TComponent>
TComponent &Registry::GetComponent(Entity entity) const {
  const auto componentId = Component<TComponent>::GetId();
  const auto entityId = entity.GetId();
  std::shared_ptr<Pool<TComponent>> componentPool =
      std::static_pointer_cast<Pool<TComponent>>(componentPools[componentId]);
  return componentPool->Get(entityId);
}

template <typename TComponent, typename... TArgs>
void Entity::AddComponent(TArgs &&...args) {
  registry->AddComponent<TComponent>(*this, std::forward<TArgs>(args)...);
};
template <typename TComponent> void Entity::RemoveComponent() {
  registry->RemoveComponent<TComponent>(*this);
};
template <typename TComponent> bool Entity::HasComponent() const {
  return registry->HasComponent<TComponent>(*this);
};
template <typename TComponent> TComponent &Entity::GetComponent() const {
  return registry->GetComponent<TComponent>(*this);
};

#endif
