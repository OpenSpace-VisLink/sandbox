#include "sandbox/SceneNode.h"
#include "sandbox/SceneComponent.h"
#include <iostream>

namespace sandbox {

SceneNode::SceneNode() {

}

SceneNode::~SceneNode() {
	for (int f = 0; f < components.size(); f++) {
		delete components[f];
	}	

	for (int f = 0; f < nodes.size(); f++) {
		delete nodes[f];
	}
}

void SceneNode::addNode(SceneNode* node) {
	nodes.push_back(node);
}

void SceneNode::addComponent(SceneComponent* component) {
	component->setSceneNode(this);
	for (int f = 0; f < component->getTypes().size(); f++) {
		typed_components[component->getTypes()[f]] = component;
	}

	components.push_back(component);
}

void SceneNode::deleteComponent(SceneComponent* component) {
	// TODO: make work
}

void SceneNode::updateModel() {
	for (int f = 0; f < components.size(); f++) {
		components[f]->updateModel();
	}

	for (int f = 0; f < nodes.size(); f++) {
		nodes[f]->updateModel();
	}
}

void SceneNode::updateSharedContext(const SceneContext& sceneContext) {
	for (int f = 0; f < components.size(); f++) {
		components[f]->updateSharedContext(sceneContext);
	}

	for (int f = 0; f < nodes.size(); f++) {
		nodes[f]->updateSharedContext(sceneContext);
	}
}

void SceneNode::updateContext(const SceneContext& sceneContext) {
	for (int f = 0; f < components.size(); f++) {
		components[f]->updateContext(sceneContext);
	}

	for (int f = 0; f < nodes.size(); f++) {
		nodes[f]->updateContext(sceneContext);
	}
}

void SceneNode::use(const SceneContext& sceneContext) {
	for (int f = 0; f < components.size(); f++) {
		components[f]->use(sceneContext);
	}
	
	for (int f = 0; f < nodes.size(); f++) {
		nodes[f]->use(sceneContext);
	}
}

void SceneNode::render(const SceneContext& sceneContext) {
	for (int f = 0; f < components.size(); f++) {
		components[f]->render(sceneContext);
	}
	
	for (int f = 0; f < nodes.size(); f++) {
		nodes[f]->render(sceneContext);
	}
}

SceneComponent* SceneNode::getComponentByType(const std::type_info& type) const {
	std::map<const std::type_info*, SceneComponent*, type_compare>::const_iterator it = typed_components.find(&type);
	if (it == typed_components.end()) {
		return NULL;
	}

	return it->second;
}

}