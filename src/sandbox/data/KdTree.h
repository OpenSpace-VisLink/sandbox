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
	virtual unsigned int getSize() const = 0;
	virtual T getDimension(unsigned int index, unsigned int dimension) const = 0;
	virtual T getDistance(std::vector<T> point, unsigned int index, std::vector<unsigned int> dimensions) const = 0;
};

template<typename T>
class KdTree {
public:
	KdTree(std::vector<unsigned int> dimensions, const KdSearchable<T>& searchable) : dimensions(dimensions), searchable(searchable) {
		for (unsigned int f = 0; f < searchable.getSize(); f++) {
			values.push_back(f);
		}

		createSplit(0, values.size(), 0);
	}

	virtual ~KdTree() {}


	struct KdValue {
		unsigned int index;
		T distance;
	};

	std::vector<KdValue> getNearest(std::vector<T> point, int num) const {
		std::vector<KdValue> values;
		values.push_back(KdValue());
		int splitSize = values.size();
		int start = 0;
		//int end = 

		while (splitSize / 2 >= num) {
			/*
			int median = start + (end - start)/2;
		int nextDimensionIndex = (dimensionIndex + 1) % dimensions.size();
		createSplit(start, median+1, nextDimensionIndex);
		createSplit(median+1, end, nextDimensionIndex);*/


			splitSize = splitSize / 2;
		}



		return values;
	}

	std::vector<KdValue> getNearestSorted(std::vector<T> point, int num) const {
		return getNearest;
	}


private:
	void createSplit(int start, int end, int dimensionIndex) {
		std::cout << "Create split " << start << "..." << end << std::endl;
		if (end - start == 1) {
			return;
		}

		std::sort(this->values.begin() + start, this->values.begin() + end, CompareDimension(searchable, dimensions[dimensionIndex]));
		int median = start + (end - start)/2;
		int nextDimensionIndex = (dimensionIndex + 1) % dimensions.size();
		createSplit(start, median, nextDimensionIndex);
		createSplit(median, end, nextDimensionIndex);
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