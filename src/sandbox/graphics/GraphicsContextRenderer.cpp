#include "sandbox/graphics/GraphicsContextRenderer.h"
#include "sandbox/graphics/RenderState.h"
#include "sandbox/base/Transform.h"

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

	render();
}

void GraphicsContextRenderer::render() {
	renderRecursive(getEntity());
}

void GraphicsContextRenderer::renderRecursive(const Entity& entity) {
	RenderState& renderState = RenderState::get(context);
	renderState.getEntity().push(&entity);

	std::vector<Transform*> transforms = entity.getComponents<Transform>();
	for (int f = 0; f < transforms.size(); f++) {
		renderState.getModelMatrix().push(renderState.getModelMatrix().get()*transforms[f]->getTransform());
	}

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

	for (int f = 0; f < transforms.size(); f++) {
		renderState.getModelMatrix().pop();
	}

	renderState.getEntity().pop();
}


/*void Transform::render(const SceneContext& sceneContext) {
	RenderState& renderState = RenderState::get(sceneContext);
	renderState.getModelMatrix().push(renderState.getModelMatrix().get()*transform);
}

void Transform::finishRender(const SceneContext& sceneContext) {
	RenderState& renderState = RenderState::get(sceneContext);
	renderState.getModelMatrix().pop();
}*/


}
