#ifndef SANDBOX_BASE_OBJECT_H_
#define SANDBOX_BASE_OBJECT_H_

#include "sandbox/Component.h"

namespace sandbox {

class ObjectBase : public Component {
public:
	ObjectBase() { addType<ObjectBase>(); }
	virtual ~ObjectBase() {}

	virtual const std::type_info& getObjectType() const = 0;
};

template<typename T>
class Object : public ObjectBase {
public:
	Object() : object() { addType< Object<T> >(); }
	Object(T& object) : object(object) { addType< Object<T> >(); }
	virtual ~Object() {}

	T& get() { return object; }
	void set(T& object) { this->object = object; }

	const std::type_info& getObjectType() const {
		static const std::type_info& type = typeid(T);
		return type;
	}

private:
	T object;
};

template<typename T>
class ObjectRef : public Object<T*> {};

template<typename T>
class ObjectPtr : public Object<T*> {
public:
	ObjectPtr(T* ptr = NULL, bool deletePtr = true) : deletePtr(deletePtr) {
		Object<T*>::set(ptr);
	}
	~ObjectPtr() {
		if (deletePtr) {
			delete Object<T*>::get();
		}
	}

private:
	bool deletePtr;

};

}

#endif