#ifndef SANDBOX_ENTITY_H_
#define SANDBOX_ENTITY_H_

#include <typeinfo>
#include <map>
#include <vector>
#include "glm/glm.hpp"

namespace sandbox {

class Component;

class Entity {
public:
	Entity();
	Entity(Entity* parent);
	virtual ~Entity();

	void addChild(Entity* entity);
	const std::vector<Entity*>& getChildren() const { return children; }
	const Entity* getParent() const { return parent; }

	void addComponent(Component* component);
	void deleteComponent(Component* component);
	const std::vector<Component*>& getComponents() const { return components; }

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
	bool visible;
};

}

#endif