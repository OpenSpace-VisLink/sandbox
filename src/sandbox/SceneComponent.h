#ifndef SANDBOX_SCENE_COMPONENT_H_
#define SANDBOX_SCENE_COMPONENT_H_

#include <typeinfo>
#include <vector>
#include "sandbox/SceneNode.h"

namespace sandbox {

class SceneComponent {
public:
	virtual ~SceneComponent() {}
	virtual void setSceneNode(SceneNode* node) { this->node = node; }
	virtual void onSceneNodeChange() {}
	virtual void updateModel() {}
	virtual void updateSharedContext(const SceneContext& sceneContext) {}
	virtual void updateContext(const SceneContext& sceneContext) {}
	virtual void use(const SceneContext& sceneContext) {}

	template<typename T>
	void addType() {
		static const std::type_info& type = typeid(T);
		types.push_back(&type);
	}
	const std::vector<const std::type_info*> getTypes() { return types; }

private:
	SceneNode* node;
	std::vector<const std::type_info*> types;
};

}

#endif