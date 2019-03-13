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

	const T* getPoint(unsigned int index) const {
		return &getArray()[index*getVariables().size()];
	}

	T getDimension(unsigned int index, unsigned int dimension) const {
		return getPoint(index)[dimension];
	}

	struct DataViewStatistics {
		DataViewStatistics() : min(), max(), mean(), variance() {}

		T getMin() const { return min; }
		T getMax() const { return max; }
		T getMean() const { return mean; }
		T getVariance() const { return variance; }
		T getStandardDeviation() const { return std::sqrt(variance); }

		T min, max, mean, variance;
	};

	virtual const DataViewStatistics& getStatistics(unsigned int dimension) const = 0;
	T getMin(unsigned int dimension) const { return getStatistics(dimension).getMin(); }
	T getMax(unsigned int dimension) const { return getStatistics(dimension).getMax(); }
	T getMean(unsigned int dimension) const { return getStatistics(dimension).getMean(); }
	T getVariance(unsigned int dimension) const { return getStatistics(dimension).getVariance(); }
	T getStandardDeviation(unsigned int dimension) const { return getStatistics(dimension).getStandardDeviation(); }
};

typedef DataView<float> FloatDataView;

template<typename T>
class NormalizedEuclideanDistance : public KdDistance<T> {
public:
	NormalizedEuclideanDistance(const DataView<T>& view) : view(view) {}

	T getDistance(const std::vector<T>& pointA, const std::vector<T>& pointB, const std::vector<unsigned int>& dimensions) const {
		T dist = T();

		for (int f = 0; f < dimensions.size(); f++) {
			dist += std::pow((pointB[f] - pointA[f])/(view.getStatistics(dimensions[f]).getMax() - view.getStatistics(dimensions[f]).getMin()), 2.0f);
		}

		return std::sqrt(dist);
	}

private:
	const DataView<T>& view;
};


}

#endif