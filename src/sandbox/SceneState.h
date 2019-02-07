#ifndef SANDBOX_SCENE_STATE_H_
#define SANDBOX_SCENE_STATE_H_

#include <typeinfo>
#include <vector>
#include <map>

namespace sandbox {

class SceneStateItem {
public:
	virtual ~SceneStateItem() {}
};

class SceneState {
public:
	virtual ~SceneState() {
		for (int f = 0; f < items.size(); f++) {
			delete items[f];
		}
	}

	template<typename T>
	T& getItem() {
		static const std::type_info& type = typeid(T);
		std::map<const std::type_info*, SceneStateItem*, type_compare>::iterator it = typed_items.find(&type);
		T* item = NULL;
		if (it == typed_items.end()) {
			item = static_cast<T*>(it->second);
		}
		else {
			item = new T();
			items.push_back(item);
			typed_items[&type] = item;
		}

		return *item;
	}

private:
	struct type_compare {
	    bool operator ()(const std::type_info* a, const std::type_info* b) const {
	        return a->before(*b);
	    }
	};
	std::map<const std::type_info*, SceneStateItem*, type_compare> typed_items;
	std::vector<SceneStateItem*> items;
};

template<typename T>
class StateItemStack {
public:
	StateItemStack() {
		items.push_back(T());
	}

	virtual ~StateItemStack() {}

	void set(const T& item) {
		items[items.size() - 1] = item;
	}

	const T& get() {
		return items[items.size() - 1];
	}

	void push(const T& item) {
		items.push_back(item);
	}

	void pop() {
		if (items.size() > 1) {
			items.pop_back();
		}
	}

private:
	std::vector<T> items;
};

}

#endif