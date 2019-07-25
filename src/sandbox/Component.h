#ifndef SANDBOX_COMPONENT_H_
#define SANDBOX_COMPONENT_H_

#include <typeinfo>
#include <vector>
#include "sandbox/Entity.h"

namespace sandbox {

class Component {
public:
	Component() : entity(NULL) {}
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

private:
	std::vector<const std::type_info*> types;
	Entity* entity;
};

}

#endif