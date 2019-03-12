#ifndef SANDBOX_DATA_VIEW_H_
#define SANDBOX_DATA_VIEW_H_

#include "sandbox/data/KdTree.h"

namespace sandbox {

template <typename T>
class DataView : public SceneComponent, public KdSearchable<T> {
public:
	DataView() {
		addType< DataView<float> >();
	}
	virtual ~DataView() {}
	virtual const std::vector<std::string>& getVariables() const = 0;
	virtual const std::vector<T>& getArray() const = 0;
	virtual T getMin(unsigned int dimension) const = 0;
	virtual T getMax(unsigned int dimension) const = 0;

	const T* getPoint(unsigned int index) const {
		return &getArray()[index*getVariables().size()];
	}
};

typedef DataView<float> FloatDataView;

}

#endif