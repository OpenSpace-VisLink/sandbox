#include "sandbox/graphics/GraphicsContextRenderer.h"
#include "sandbox/graphics/RenderState.h"
#include "sandbox/base/Transform.h"

namespace sandbox {

void GraphicsContextRenderer::update() {
	/*std::vector<const Entity*> entityStack;
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
	}*/
	update(getEntity());

}


void GraphicsContextRenderer::render() {
	RenderState& renderState = RenderState::get(context);
	renderState.getRenderer().push(this);
	render(getEntity());
	renderState.getRenderer().pop();
}

void GraphicsContextRenderer::update(const Entity& entity) {
	std::vector<GraphicsComponent*> components = entity.getComponents<GraphicsComponent>();
	for (int f = 0; f < components.size(); f++) {
		components[f]->updateSharedContext(context);
		components[f]->updateContext(context);
	}

	for (int f = 0; f < entity.getChildren().size(); f++) {
		update(*entity.getChildren()[f]);
	}
}

void GraphicsContextRenderer::render(const Entity& entity) {
	startRender(entity);
	renderChildren(entity);
	finishRender(entity);
}

void GraphicsContextRenderer::startRender(const Entity& entity) {
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
}

void GraphicsContextRenderer::renderChildren(const Entity& entity) {
	for (int f = 0; f < entity.getChildren().size(); f++) {
		render(*entity.getChildren()[f]);
	}
}

void GraphicsContextRenderer::finishRender(const Entity& entity) {
	RenderState& renderState = RenderState::get(context);
	
	std::vector<GraphicsComponent*> components = entity.getComponents<GraphicsComponent>();
	for (int f = 0; f < components.size(); f++) {
		components[f]->finishRender(context);
	}

	std::vector<Transform*> transforms = entity.getComponents<Transform>();
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
