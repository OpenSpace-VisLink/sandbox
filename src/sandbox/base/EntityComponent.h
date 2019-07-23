#ifndef SANDBOX_BASE_ENTITY_COMPONENT_H_
#define SANDBOX_BASE_ENTITY_COMPONENT_H_

#include "sandbox/Component.h"

namespace sandbox {

class EntityComponent : public Component {
public:
	EntityComponent(Entity* entity) : entity(entity), lastUpdateVersion(entity->getVersion()) { addType<EntityReference>(); }
	virtual ~EntityComponent() {}

	Entity* getEntityReference() const { return entity; }

	void prepareUpdate() {
		if (lastUpdateVersion != entity->getVersion()) {
			getEntity().incrementVersion();
			lastUpdateVersion = entity->getVersion();
		}
	}

private:
	Entity* entity;
	int lastUpdateVersion;
};

}

#endif