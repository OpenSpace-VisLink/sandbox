#include "sandbox/Entity.h"
#include "sandbox/Component.h"
#include "sandbox/base/Transform.h"
#include <iostream>
#include "sandbox/graphics/RenderState.h"

namespace sandbox {

Entity::Entity(Entity* parent) : parent(parent), visible(true) {
	parent->addChild(this);
}

Entity::Entity() : parent(NULL), visible(true) {

}

Entity::~Entity() {
	for (int f = 0; f < components.size(); f++) {
		delete components[f];
	}	

	for (int f = 0; f < children.size(); f++) {
		delete children[f];
	}
}

void Entity::addChild(Entity* entity) {
	entity->parent = this;
	children.push_back(entity);
}

void Entity::addComponent(Component* component) {
	component->setEntity(this);
	for (int f = 0; f < component->getTypes().size(); f++) {

		std::map<const std::type_info*, std::vector<Component*>, type_compare>::const_iterator it = typed_components.find(component->getTypes()[f]);
		if (it == typed_components.end()) {
			typed_components[component->getTypes()[f]] = std::vector<Component*>();
		}

		typed_components[component->getTypes()[f]].push_back(component);
	}

	components.push_back(component);
}

void Entity::deleteComponent(Component* component) {
	// TODO: make work
}

Component* Entity::getComponentByType(const std::type_info& type) const {
	const std::vector<Component*>& components = Entity::getComponentsByType(type);
	if(components.size() == 0) {
		return NULL;
	}

	return components[0];
}

 const std::vector<Component*>& Entity::getComponentsByType(const std::type_info& type) const {
	static std::vector<Component*> emptyArray;

	std::map<const std::type_info*, std::vector<Component*>, type_compare>::const_iterator it = typed_components.find(&type);
	if (it == typed_components.end()) {
		return emptyArray;
	}

	return it->second;
}

}