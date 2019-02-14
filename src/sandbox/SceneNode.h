#ifndef SANDBOX_SCENE_NODE_H_
#define SANDBOX_SCENE_NODE_H_

#include <typeinfo>
#include <map>
#include <vector>
#include "sandbox/SceneContext.h"
#include "glm/glm.hpp"

namespace sandbox {

class SceneComponent;

class SceneNode {
public:
	SceneNode();
	virtual ~SceneNode();

	void addNode(SceneNode* node);
	const std::vector<SceneNode*>& getChildren() const { return nodes; }
	const SceneNode* getParent() const { return parent; }

	void addComponent(SceneComponent* component);
	void deleteComponent(SceneComponent* component);
	const std::vector<SceneComponent*>& getComponents() const { return components; }

	void updateModel();
	void updateSharedContext(const SceneContext& sceneContext);
	void updateContext(const SceneContext& sceneContext);
	void use(const SceneContext& sceneContext);
	void render(const SceneContext& sceneContext);

	glm::vec3 getWorldPosition() const;

	template<typename T>
	T* getComponent() const {
		static const std::type_info& type = typeid(T);
		SceneComponent* component = getComponentByType(type);
		if (component) {
			return static_cast<T*>(component);
		}

		return NULL;
	}

	bool isVisible() { return visible; }
	void setVisible(bool visible) { this->visible = visible; }

protected:
	SceneComponent* getComponentByType(const std::type_info& type) const;

private:
	struct type_compare {
	    bool operator ()(const std::type_info* a, const std::type_info* b) const {
	        return a->before(*b);
	    }
	};
	std::map<const std::type_info*, SceneComponent*, type_compare> typed_components;
	std::vector<SceneComponent*> components;
	std::vector<SceneNode*> nodes;
	SceneNode* parent;
	bool visible;
};

}

#endif