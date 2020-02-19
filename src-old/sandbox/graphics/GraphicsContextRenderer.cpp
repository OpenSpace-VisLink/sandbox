#include "sandbox/graphics/GraphicsContextRenderer.h"

namespace sandbox {

void GraphicsContextRenderer::update() {
	std::vector<const Entity*> entityStack;
	entityStack.push_back(&getEntity());

	while (entityStack.size() > 0) {
		const Entity& entity = *entityStack[entityStack.size()-1];
		std::vector<GraphicsComponent*> components = entity.getComponents<GraphicsComponent>();
		for (int f = 0; f < components.size(); f++) {
			components[f]->updateSharedContext(context);
			components[f]->updateContext(context);
		}

		entityStack.pop_back();

		for (int f = entity.getChildren().size()-1; f >= 0; f--) {
			entityStack.push_back(entity.getChildren()[f]);
		}
	}
}

void GraphicsContextRenderer::render() {
	renderRecursive(getEntity());
}

void GraphicsContextRenderer::renderRecursive(const Entity& entity) {
	std::vector<GraphicsComponent*> components = entity.getComponents<GraphicsComponent>();
	for (int f = 0; f < components.size(); f++) {
		components[f]->startRender(context);
	}

	for (int f = 0; f < entity.getChildren().size(); f++) {
		renderRecursive(*entity.getChildren()[f]);
	}

	for (int f = 0; f < components.size(); f++) {
		components[f]->finishRender(context);
	}
}

}
