#include "sandbox/Entity.h"
#include "sandbox/Component.h"

namespace sandbox {

EntityNode::EntityNode(Entity* parent) : parent(parent), version(0), lastUpdateVersion(-1) {
	parent->addChild(this);
}

EntityNode::EntityNode() : parent(NULL), version(0), lastUpdateVersion(-1) {

}

EntityNode::~EntityNode() {
	for (int f = 0; f < children.size(); f++) {
		delete children[f];
	}

	for (int f = components.size()-1; f >= 0; f--) {
		delete components[f];
	}	
}

void EntityNode::update() {
	for (int f = 0; f < components.size(); f++) {
		components[f]->prepareUpdate();
	}

	bool updated = lastUpdateVersion != version;

	for (int f = 0; f < components.size(); f++) {
		if (updated || components[f]->forceUpdate()) {
			components[f]->update();
		}
	}

	if (updated) {
		for (int f = 0; f < children.size(); f++) {
			children[f]->incrementVersion();
		}

		lastUpdateVersion = version;
	}


	for (int f = 0; f < children.size(); f++) {
		children[f]->update();
	}
}

void EntityNode::addChild(Entity* entity) {
	entity->setParent(this);
	children.push_back(entity);
	incrementVersion();
}

void EntityNode::addComponent(Component* component) {
	component->setEntity(this);

	bool componentIsAdded = component->beforeAdd();
	if (componentIsAdded) {
		for (int f = 0; f < component->getTypes().size(); f++) {

			std::map<const std::type_info*, std::vector<Component*>, type_compare>::const_iterator it = typed_components.find(component->getTypes()[f]);
			if (it == typed_components.end()) {
				typed_components[component->getTypes()[f]] = std::vector<Component*>();
			}

			typed_components[component->getTypes()[f]].push_back(component);
		}

		components.push_back(component);
	}
	
	component->afterAdd();

	if (!componentIsAdded) {
		delete component;
	}

	incrementVersion();
}

void EntityNode::deleteComponent(Component* component) {
	// TODO: make work
	incrementVersion();
}

Component* EntityNode::getComponentByType(const std::type_info& type) const {
	const std::vector<Component*>& components = EntityNode::getComponentsByType(type);
	if(components.size() == 0) {
		return NULL;
	}

	return components[0];
}

 const std::vector<Component*>& EntityNode::getComponentsByType(const std::type_info& type) const {
	static std::vector<Component*> emptyArray;

	std::map<const std::type_info*, std::vector<Component*>, type_compare>::const_iterator it = typed_components.find(&type);
	if (it == typed_components.end()) {
		return emptyArray;
	}

	return it->second;
}


}