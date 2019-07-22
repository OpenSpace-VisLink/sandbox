#ifndef SANDBOX_ENTITY_H_
#define SANDBOX_ENTITY_H_

#include <typeinfo>
#include <map>
#include <vector>
#include "glm/glm.hpp"

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
};

class EntityReference : public Entity {
public:
	EntityReference(Entity* entity) : entity(entity), parent(NULL) {}
	EntityReference(Entity* entity, Entity* parent) : entity(entity), parent(parent) {}
	virtual ~EntityReference() {}
	virtual void addChild(Entity* entity) { entity->addChild(entity); }
	virtual const std::vector<Entity*>& getChildren() const { return entity->getChildren(); }
	virtual const Entity* getParent() const  { return parent; }
	virtual void addComponent(Component* component) { return entity->addComponent(component); }
	virtual void deleteComponent(Component* component) { return entity->deleteComponent(component); }
	virtual const std::vector<Component*>& getComponents() const { return entity->getComponents(); }
	virtual void update() { return entity->update(); }
protected:
	virtual Component* getComponentByType(const std::type_info& type) const { return entity->getComponentByType(type); }
	virtual const std::vector<Component*>& getComponentsByType(const std::type_info& type) const { return entity->getComponentsByType(type); }
	virtual void setParent(Entity* entity) { parent = entity; }

private:
	Entity* entity;
	Entity* parent;
};

}

#endif