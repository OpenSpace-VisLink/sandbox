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
		T getStandardDeviation() const { return standardDeviation; }

		T min, max, mean, variance, standardDeviation;
	};

	virtual const DataViewStatistics& getStatistics(unsigned int dimension) const = 0;
	T getMin(unsigned int dimension) const { return getStatistics(dimension).getMin(); }
	T getMax(unsigned int dimension) const { return getStatistics(dimension).getMax(); }
	T getMean(unsigned int dimension) const { return getStatistics(dimension).getMean(); }
	T getVariance(unsigned int dimension) const { return getStatistics(dimension).getVariance(); }
	T getStandardDeviation(unsigned int dimension) const { return getStatistics(dimension).getStandardDeviation(); }

	std::vector<DataViewStatistics> calculateStatistics() const {
		std::vector<DataViewStatistics> statistics;
		const std::vector<unsigned int>& points = this->getPoints();

		for (int f = 0; f < getVariables().size(); f++) {
			statistics.push_back(calculateStatistics(f));
		}

		return statistics;
	}

	virtual DataViewStatistics calculateStatistics(unsigned int dimension) const {
		DataViewStatistics stats;
		const std::vector<unsigned int>& points = this->getPoints();

		for (int i = 0; i < points.size(); i++) {
			T val = getDimension(points[i], dimension);
			if (i == 0) {
				stats.min = val;
				stats.max = val;
			}
			else {
				if (stats.min > val) { stats.min = val; }
				if (stats.max < val) { stats.max = val; }
			}

			stats.mean += val;
		}

		stats.mean /= points.size();

		for (int i = 0; i < points.size(); i++) {
			T val = getDimension(points[i], dimension);
			T diff = val - stats.mean;
			stats.variance += diff*diff;
		}

		stats.variance /= points.size();
		stats.standardDeviation = std::sqrt(stats.variance);

		return stats;
	}

	/*
	   			function calcStatistics(items, getValue) {
   				var mean = 0;
   				var max = 0;
   				var min = 0;
   				items.forEach(function(item, index) {
   					var val = getValue(item);
   					if (index == 0) {
   						max = val;
   						min = val;
   					}
   					else {
   						max = max < val ? val : max;
   						min = min > val ? val : min;
   					}
   					mean += val;
   				});
   				mean /= items.length;

   				var variance = 0;
   				items.forEach(function(item, index) {
   					var val = getValue(item);
   					variance += (val - mean)*(val-mean);
   				});
   				variance /= items.length;

   				return {mean: mean, variance: variance, max: max, min: min};
      		}
      		*/
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