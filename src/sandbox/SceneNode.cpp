#include "sandbox/SceneNode.h"
#include "sandbox/SceneComponent.h"
#include "sandbox/base/Transform.h"
#include <iostream>
#include "sandbox/graphics/RenderState.h"

namespace sandbox {

SceneNode::SceneNode(SceneNode* parent) : parent(parent), visible(true) {
	parent->addNode(this);
}

SceneNode::SceneNode() : parent(NULL), visible(true) {

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
	node->parent = this;
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
	if (!visible) { return; }

	RenderState::get(sceneContext).getSceneNode().push(this);

	for (int f = 0; f < components.size(); f++) {
		components[f]->render(sceneContext);
	}
	
	for (int f = 0; f < nodes.size(); f++) {
		nodes[f]->render(sceneContext);
	}

	for (int f = components.size()-1; f >= 0; f--) {
		components[f]->finishRender(sceneContext);
	}

	RenderState::get(sceneContext).getSceneNode().pop();
}

glm::vec3 SceneNode::getWorldPosition() const {
	glm::vec4 position(0.0f, 0.0f, 0.0f, 1.0f);
	for (const SceneNode* node = this; node != NULL; node = node->getParent() ) {
		Transform* transform = node->getComponent<Transform>();
		if (transform) {
			position = transform->getTransform()*position;
		}
	}
	return position;
}


glm::mat4 SceneNode::getWorldTransform() const {
	glm::mat4 trans(1.0);
	for (const SceneNode* node = this; node != NULL; node = node->getParent() ) {
		Transform* transform = node->getComponent<Transform>();
		if (transform) {
			trans = transform->getTransform()*trans;
		}
	}

	return trans;
}

SceneComponent* SceneNode::getComponentByType(const std::type_info& type) const {
	std::map<const std::type_info*, SceneComponent*, type_compare>::const_iterator it = typed_components.find(&type);
	if (it == typed_components.end()) {
		return NULL;
	}

	return it->second;
}

}