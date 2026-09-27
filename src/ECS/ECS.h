#ifndef ECS_H
#define ECS_H
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
  virtual ~IPool();
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

  std::vector<IPool *>
      componentPools; // index into this by component type id (not component id)
  std::vector<Signature>
      entityComponentSignatures; // index into this by entity id
  std::unordered_map<std::type_index, System *> systems;

public:
  Registry() = default;
  void Update();

  // Entity management
  Entity CreateEntity();
  void AddEntityToSystem(Entity entity);

// Component management
  template <typename TComponent, typename... TArgs>
  void AddComponent(Entity entity, TArgs &&...args);
  template <typename Tcomponent>
  void RemoveComponent(Entity entity);
  template <typename Tcomponent>
  bool HasComponent(Entity entity) const;

  // System Management
};

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
  componentPools[componentId] = new Pool<TComponent>();
  Pool<TComponent> *componentPool = componentPools[componentId];

  if (entityId >= componentPool->Size()) {
    componentPool->Resize(entityId + 1);
  }

  TComponent newComponent(std::forward<TArgs>(args)...);

  componentPool->Set(entityId, newComponent);

  entityComponentSignatures[entityId].set(componentId);
}

template <typename TComponent>
void Registry::RemoveComponent(Entity entity) {
  const auto componentId = Component<Tcomponent>::GetId();
  const auto entityId = entity.GetId();

  entityComponentSignatures[entityId].set(componentId, false);
}

template <typename TComponent>
void Registry::HasComponent(Entity entity) {
  const auto componentId = Component<Tcomponent>::GetId();
  const auto entityId = entity.GetId();

  return entityComponentSignatures[entityId].test(componentId);
}

#endif
