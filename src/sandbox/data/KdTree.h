#ifndef SANDBOX_DATA_KDTREE_H_
#define SANDBOX_DATA_KDTREE_H_

#include <vector>
#include <iostream>
#include <algorithm>

namespace sandbox {

template<typename T>
class KdSearchable {
public:
	virtual ~KdSearchable() {}
	virtual const std::vector<unsigned int>& getPoints() const = 0;
	virtual T getDimension(unsigned int index, unsigned int dimension) const = 0;
};

template<typename T>
class KdDistance {
public:
	virtual ~KdDistance() {}
	virtual T getDistance(const std::vector<T>& pointA, const std::vector<T>& pointB, const std::vector<unsigned int>& dimensions) const = 0;
};

template<typename T>
class KdTree {
public:
	KdTree(std::vector<unsigned int> dimensions, const KdSearchable<T>& searchable, const KdDistance<T>* distanceFunction) : dimensions(dimensions), searchable(searchable), distanceFunction(distanceFunction) {
		values = searchable.getPoints();

		createSplit(0, values.size(), 0);
	}

	virtual ~KdTree() {
		delete distanceFunction;
	}


	struct KdValue {
		unsigned int index;
		T distance;
		bool operator <(const KdValue &b) const {
			return distance < b.distance;
		}
	};

	std::vector<KdValue> getNearest(const std::vector<T>& point, int num) const {
		std::vector<KdValue> nearest;
		//values.push_back(KdValue());
		//int splitSize = values.size();
		//int start = 0;
		//int end = 
		getNearest(point, num, nearest, 0, values.size(), 0);

		return nearest;
	}

	std::vector<KdValue> getNearestSorted(std::vector<T> point, int num) const {
		std::vector<KdValue> nearest = getNearest(point, num);
		std::sort_heap(nearest.begin(), nearest.end());
		//std::reverse(nearest.begin(), nearest.end());

		/*for (int f = 0; f < nearest.size(); f++) {
			std::cout << nearest[f].index << ", " << nearest[f].distance << " [" ;
			for (int i = 0; i < dimensions.size(); i++) {
				std::cout << searchable.getDimension(nearest[f].index, dimensions[i]) << ", ";
			}

			std::cout << "]" << std::endl;
		}*/

		return nearest;
	}


private:
	void createSplit(int start, int end, int dimensionIndex) {
		std::cout << "Create split " << start << "..." << end << std::endl;
		if (end - start <= 1) {
			return;
		}

		std::sort(this->values.begin() + start, this->values.begin() + end, CompareDimension(searchable, dimensions[dimensionIndex]));
		int median = start + (end - start)/2;
		int nextDimensionIndex = (dimensionIndex + 1) % dimensions.size();
		createSplit(start, median, nextDimensionIndex);
		createSplit(median+1, end, nextDimensionIndex);
	}

	void getNearest(const std::vector<T>& point, int num, std::vector<KdValue>& heap, int start, int end, int dimensionIndex) const {
		int median = start + (end - start)/2;
		std::vector<T> linearPoint(point.size());
		std::vector<T> projectedPoint(point.size());
		for (int f = 0; f < dimensions.size(); f++) {
			projectedPoint[f] = searchable.getDimension(values[median], dimensions[f]);
			if (f == dimensionIndex) {
				linearPoint[f] = point[f];
			}
			else {
				linearPoint[f] = projectedPoint[f];
			}
		}
		T medianDistance = distanceFunction->getDistance(point, projectedPoint, dimensions);
		T linearDistance = distanceFunction->getDistance(linearPoint, projectedPoint, dimensions);

		//T medianVal = searchable.getDimension(values[median], dimensionIndex);
		int bestStart, bestEnd, otherStart, otherEnd;
		int leftStart = start;
		int leftEnd = median;
		int rightStart = median+1;
		int rightEnd = end;
		bool hasLeft = leftEnd - leftStart > 0;
		bool hasRight = rightEnd - rightStart > 0;
		bool hasBest = true;
		bool hasOther = true;

		if (hasLeft && hasRight) {
			if (point[dimensionIndex] < searchable.getDimension(values[median], dimensions[dimensionIndex])) {
				bestStart = leftStart;
				bestEnd = leftEnd;
				otherStart = rightStart;
				otherEnd = rightEnd;
			}
			else {
				bestStart = rightStart;
				bestEnd = rightEnd;
				otherStart = leftStart;
				otherEnd = leftEnd;
			}
		}
		else if (!hasLeft && !hasRight) {
			hasBest = false;
			hasOther = false;
		}
		else if (!hasLeft) {
			bestStart = rightStart;
			bestEnd = rightEnd;
			hasOther = false;
		}
		else if (!hasRight) {
			bestStart = leftStart;
			bestEnd = leftEnd;
			hasOther = false;
		}
		
		int nextDimensionIndex = (dimensionIndex + 1) % dimensions.size();
		if (hasBest) {
			getNearest(point, num, heap, bestStart, bestEnd, nextDimensionIndex);
		}

		if (heap.size() < num || medianDistance < heap[0].distance) {
			KdValue val;
			val.distance = medianDistance;
			val.index = values[median];
			heap.push_back(val);
			std::push_heap(heap.begin(), heap.end());
			if (heap.size() > num) {
				std::pop_heap(heap.begin(), heap.end());
				heap.pop_back();
			}
		}

		if (hasOther && (heap.size() < num || linearDistance < heap[0].distance)) {
			getNearest(point, num, heap, otherStart, otherEnd, nextDimensionIndex);
		}
	}

	struct CompareDimension : std::binary_function<unsigned int, unsigned int, bool>
	{
	    CompareDimension(const KdSearchable<T>& searchable, unsigned int dimension)
	    : searchable(searchable), dimension(dimension) {}

	    bool operator()(unsigned int Lhs, unsigned int Rhs) const
	    {
	    	return searchable.getDimension(Lhs, dimension) < searchable.getDimension(Rhs, dimension);
	    }

	    const KdSearchable<T>& searchable;
	    unsigned int dimension;
	};

	std::vector<unsigned int> dimensions;
	const KdSearchable<T>& searchable;
	const KdDistance<T>* distanceFunction;
	std::vector<unsigned int> values;
};

}

/*struct CompareFloatDataSetVariable : std::binary_function<unsigned int, unsigned int, bool>
{
    CompareFloatDataSetVariable(FloatDataSet* data, int index, bool sortDesc)
    : data(data), index(index), sortDesc(sortDesc) {
    	numVariables = data->getVariables().size();
    }

    bool operator()(unsigned int Lhs, unsigned int Rhs)const
    {
    	bool compare = data->getArray()[numVariables*Lhs + index] < data->getArray()[numVariables*Rhs + index];
    	return sortDesc ? !compare : compare;
    }

    FloatDataSet* data;
    int numVariables;
    int index;
    bool sortDesc;
};

void FloatDataRenderer::sortByVariable(int index, bool sortDesc) {
	if (index > 0) {
	    this->sortedIndices = fullIndices;
	    std::sort(this->sortedIndices.begin(), this->sortedIndices.end(), CompareFloatDataSetVariable(data, index, sortDesc));
	}
	else {
	    this->sortedIndices = fullIndices;
	}

	updateElementVersion++;
}
*/

#endif