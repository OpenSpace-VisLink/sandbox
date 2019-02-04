#ifndef SANDBOX_SCENE_NODE_H_
#define SANDBOX_SCENE_NODE_H_

#include <typeinfo>
#include <map>
#include <vector>
#include "sandbox/SceneContext.h"

namespace sandbox {

class SceneComponent;

class SceneNode {
public:
	SceneNode();
	virtual ~SceneNode();

	void addNode(SceneNode* node);
	const std::vector<SceneNode*>& getChildren() const { return nodes; }

	void addComponent(SceneComponent* component);
	void deleteComponent(SceneComponent* component);
	const std::vector<SceneComponent*>& getComponents() const { return components; }

	void updateModel();
	void updateSharedContext(const SceneContext& sceneContext);
	void updateContext(const SceneContext& sceneContext);
	void use(const SceneContext& sceneContext);

	template<typename T>
	T* getComponent() const {
		static const std::type_info& type = typeid(T);
		return static_cast<T*>(getComponentByType(type));
	}

protected:
	SceneComponent* getComponentByType(const std::type_info& type) const;

private:
	std::map<const std::type_info*,SceneComponent*> typed_components;
	std::vector<SceneComponent*> components;
	std::vector<SceneNode*> nodes;
};

}

#endif