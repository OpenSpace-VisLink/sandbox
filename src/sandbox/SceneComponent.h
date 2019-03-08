#ifndef SANDBOX_SCENE_COMPONENT_H_
#define SANDBOX_SCENE_COMPONENT_H_

#include <typeinfo>
#include <vector>
#include "sandbox/SceneNode.h"

namespace sandbox {

class SceneComponent {
public:
	SceneComponent() : version(0) {}
	virtual ~SceneComponent() {}
	virtual void setSceneNode(SceneNode* node) { this->node = node; }
	virtual void onSceneNodeChange() {}
	virtual void updateModel() {}
	virtual void updateSharedContext(const SceneContext& sceneContext) {}
	virtual void updateContext(const SceneContext& sceneContext) {}
	virtual void use(const SceneContext& sceneContext) {}
	virtual void release(const SceneContext& sceneContext) {}
	virtual void render(const SceneContext& sceneContext) {}
	virtual void finishRender(const SceneContext& sceneContext) {}

	virtual long getVersion() { return version; }

	template<typename T>
	void addType() {
		static const std::type_info& type = typeid(T);
		types.push_back(&type);
	}
	const std::vector<const std::type_info*> getTypes() { return types; }

protected:
	const SceneNode& getSceneNode() const { return *node; }
	virtual void updateVersion() { version++; }

private:
	SceneNode* node;
	std::vector<const std::type_info*> types;
	long version;
};

}

#endif