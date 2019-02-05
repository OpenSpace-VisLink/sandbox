#include "sandbox/base/Transform.h"

namespace sandbox {


Transform::Transform() : transform(1.0f) {
	addType<Transform>();
}

Transform::~Transform() {

}

}