#ifndef SANDBOX_ENTITY_H_
#define SANDBOX_ENTITY_H_

#include <typeinfo>
#include <map>
#include <vector>
#include "glm/glm.hpp"
#include <mutex>

namespace sandbox {

class Component;

class Entity {
friend class EntityReference;
friend class EntityNode;
public:
	virtual ~Entity() {}
	virtual void addChild(Entity* entity) = 0;
	virtual const std::vector<Entity*>& getChildren() const = 0;
	virtual const Entity* getParent() const = 0;
	virtual void addComponent(Component* component) = 0;
	virtual void deleteComponent(Component* component) = 0;
	virtual const std::vector<Component*>& getComponents() const = 0;
	virtual void update() = 0;
	virtual void incrementVersion() = 0;
	virtual int getVersion() = 0;

	template<typename T>
	T* getComponent() const {
		static const std::type_info& type = typeid(T);
		Component* component = getComponentByType(type);
		if (component) {
			return static_cast<T*>(component);
		}

		return NULL;
	}

	template<typename T>
	std::vector<T*> getComponents() const {
		static const std::type_info& type = typeid(T);
		const std::vector<Component*>& components = getComponentsByType(type);
		std::vector<T*> typed;
		for (int f = 0; f < components.size(); f++) {
			typed.push_back(static_cast<T*>(components[f]));
		}

		return typed;
	}


	template<typename T>
	std::vector<T*> getComponentsRecursive() const {
		std::vector<T*> components = getComponents<T>();
		const std::vector<Entity*>& children = getChildren();
		for (int f = 0; f < children.size(); f++) {
			std::vector<T*> childComponents = children[f]->getComponentsRecursive<T>();
			components.insert( components.end(), childComponents.begin(), childComponents.end() );
		}

		return components;
	}

protected:
	virtual Component* getComponentByType(const std::type_info& type) const = 0;
	virtual const std::vector<Component*>& getComponentsByType(const std::type_info& type) const = 0;
	virtual void setParent(Entity* entity) = 0;
};

class EntityNode : public Entity {
public:
	EntityNode();
	EntityNode(Entity* parent);
	virtual ~EntityNode();

	void addChild(Entity* entity);
	const std::vector<Entity*>& getChildren() const { return children; }
	const Entity* getParent() const { return parent; }

	void addComponent(Component* component);
	void deleteComponent(Component* component);
	const std::vector<Component*>& getComponents() const { return components; }

	void update();
	void incrementVersion() { 
		version++; 
	}
	int getVersion() { return version; }

	template<typename T>
	T* getComponent() const {
		static const std::type_info& type = typeid(T);
		Component* component = getComponentByType(type);
		if (component) {
			return static_cast<T*>(component);
		}

		return NULL;
	}

	template<typename T>
	std::vector<T*> getComponents() const {
		static const std::type_info& type = typeid(T);
		const std::vector<Component*>& components = getComponentsByType(type);
		std::vector<T*> typed;
		for (int f = 0; f < components.size(); f++) {
			typed.push_back(static_cast<T*>(components[f]));
		}

		return typed;
	}

protected:
	Component* getComponentByType(const std::type_info& type) const;
	const std::vector<Component*>& getComponentsByType(const std::type_info& type) const;
	void setParent(Entity* entity) { this->parent = entity; }

private:
	struct type_compare {
	    bool operator ()(const std::type_info* a, const std::type_info* b) const {
	        return a->before(*b);
	    }
	};
	std::map<const std::type_info*, std::vector<Component*>, type_compare> typed_components;
	std::vector<Component*> components;
	std::vector<Entity*> children;
	Entity* parent;
	int version;
	int lastUpdateVersion;
};

class EntityReference : public Entity {
public:
	EntityReference(Entity* entity) : entity(entity), parent(NULL) {}
	EntityReference(Entity* entity, Entity* parent) : entity(entity), parent(parent) {}
	virtual ~EntityReference() {}
	void addChild(Entity* entity) { entity->addChild(entity); }
	const std::vector<Entity*>& getChildren() const { return entity->getChildren(); }
	const Entity* getParent() const  { return parent; }
	void addComponent(Component* component) { return entity->addComponent(component); }
	void deleteComponent(Component* component) { return entity->deleteComponent(component); }
	const std::vector<Component*>& getComponents() const { return entity->getComponents(); }
	void update() { return entity->update(); }
	void incrementVersion() { entity->incrementVersion(); }
	int getVersion() { return entity->getVersion(); }
protected:
	Component* getComponentByType(const std::type_info& type) const { return entity->getComponentByType(type); }
	const std::vector<Component*>& getComponentsByType(const std::type_info& type) const { return entity->getComponentsByType(type); }
	void setParent(Entity* entity) { parent = entity; }

private:
	Entity* entity;
	Entity* parent;
};

class ReadOnlyEntityReference : public EntityReference {
public:
	ReadOnlyEntityReference(Entity* entity) : EntityReference(entity) {}
	ReadOnlyEntityReference(Entity* entity, Entity* parent) : EntityReference(entity, parent) {}
	void update() {}
	void incrementVersion() {}
};

}

#endif