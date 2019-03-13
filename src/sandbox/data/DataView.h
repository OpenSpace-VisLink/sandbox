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

	T getDimension(unsigned int index, unsigned int dimension) const {
		return getPoint(index)[dimension];
	}
};

typedef DataView<float> FloatDataView;

template<typename T>
class NormalizedEuclideanDistance : public KdDistance<T> {
public:
	NormalizedEuclideanDistance(const DataView<T>& view) : view(view) {}

	T getDistance(const std::vector<T>& pointA, const std::vector<T>& pointB, const std::vector<unsigned int>& dimensions) const {
		T dist = T();

		for (int f = 0; f < dimensions.size(); f++) {
			dist += std::pow((pointB[f] - pointA[f])/(view.getMax(dimensions[f]) - view.getMin(dimensions[f])), 2.0f);
		}

		return std::sqrt(dist);
	}

private:
	const DataView<T>& view;
};


}

#endif