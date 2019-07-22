#ifndef SANDBOX_STATE_CONTAINER_H_
#define SANDBOX_STATE_CONTAINER_H_

#include <typeinfo>
#include <vector>
#include <map>

namespace sandbox {

class StateContainerItem {
public:
	virtual ~StateContainerItem() {}
};

class StateContainer {
public:
	virtual ~StateContainer() {
		for (int f = 0; f < items.size(); f++) {
			delete items[f];
		}
	}

	template<typename T>
	T& getItem() {
		static const std::type_info& type = typeid(T);
		std::map<const std::type_info*, StateContainerItem*, type_compare>::iterator it = typed_items.find(&type);
		T* item = NULL;
		if (it != typed_items.end()) {
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
	std::map<const std::type_info*, StateContainerItem*, type_compare> typed_items;
	std::vector<StateContainerItem*> items;
};

template<typename T>
class StateItemStack {
public:
	StateItemStack() : finalIndex(-1) {
		items.push_back(T());
	}

	virtual ~StateItemStack() {}

	void set(const T& item) {
		items[items.size() - 1] = item;
	}

	const T& get() {
		if (finalIndex >= 0) {
			return items[finalIndex];
		}

		return items[items.size() - 1];
	}

	void push(const T& item, bool final = false) {
		if (final) {
			finalIndex = items.size();
		}

		items.push_back(item);
	}

	void pop() {
		if (items.size() > 1) {
			items.pop_back();
		}

		if (finalIndex == items.size()) {
			finalIndex = -1;	
		}
	}

private:
	std::vector<T> items;
	int finalIndex;
};

}

#endif