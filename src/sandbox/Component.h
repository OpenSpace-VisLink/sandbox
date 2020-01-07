#ifndef SANDBOX_COMPONENT_H_
#define SANDBOX_COMPONENT_H_

#include <typeinfo>
#include <vector>
#include "sandbox/Entity.h"

namespace sandbox {

class Component {
public:
	Component() : entity(NULL), version(0) {}
	virtual ~Component() {}

	void setEntity(Entity* entity) {
		this->entity = entity;
	}

	template<typename T>
	void addType() {
		static const std::type_info& type = typeid(T);
		types.push_back(&type);
	}
	const std::vector<const std::type_info*> getTypes() { return types; }

	virtual bool beforeAdd() { return true; }
	virtual void afterAdd() {}
	virtual void prepareUpdate() {}
	virtual void update() {}
	virtual bool forceUpdate() { return false; }
	Entity& getEntity() const { return *entity; }
	virtual void incrementVersion() { version++; if (entity) { entity->incrementVersion(); } }
	virtual unsigned int getVersion() const { return version; }

private:
	std::vector<const std::type_info*> types;
	Entity* entity;
	unsigned int version;
};

class ComponentDependency {
public:
	ComponentDependency(const Component* component = NULL) {
		reset(component);
	}

	void reset(const Component* component = NULL) {
		this->component = component;
		version = component ? component->getVersion() : 0;
		initialized = false;
	}

	bool isCurrent(const Component* component) {
		if (this->component != component) {
			reset(component);
		}

		bool current = !component || (initialized && version == component->getVersion());

		if (component && !current) {
			version = component->getVersion();
			initialized = true;
		}

		return current;
	}

private:
	const Component* component;
	bool initialized;
	unsigned int version;
};

}

#endif