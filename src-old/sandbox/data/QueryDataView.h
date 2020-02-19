#ifndef SANDBOX_QUERY_DATA_VIEW_H_
#define SANDBOX_QUERY_DATA_VIEW_H_

#include "sandbox/data/DataViewDecorator.h"
#include "sandbox/SceneNode.h"
#include <algorithm>

namespace sandbox {

template <typename T>
class DataViewQuery {
public:
	virtual void updatePoints(const DataView<T>& view, std::vector<unsigned int>& points) const {}
};

template <typename T>
class AdvancedDataView : public DataViewDecorator<T> {
public:
	AdvancedDataView(SceneNode* node) : DataViewDecorator<T>(node) {
		SceneComponent::addType< AdvancedDataView<T> >();
	}

	void updateModel() {
		DataViewDecorator<T>::updateModel();
		const DataView<T>* view = DataViewDecorator<T>::getView();
		if (view && version != SceneComponent::getVersion()) {
			points = view->getPoints();

			for (int f = 0; f < queries.size(); f++) {
				queries[f]->updatePoints(*view, points);
			}

			version = SceneComponent::getVersion();
		}
	}

	virtual ~AdvancedDataView() {
		for (int f = 0; f < queries.size(); f++) {
			delete queries[f];
		}
	}

	void addQuery(DataViewQuery<T>* query) {
		queries.push_back(query);
		SceneComponent::updateVersion();
	}

	const std::vector<unsigned int>& getPoints() const { return points; }

private:
	std::vector<DataViewQuery<T>*> queries;
	long version;
	std::vector<unsigned int> points;
};

typedef AdvancedDataView<float> FloatAdvancedDataView;
typedef DataViewQuery<float> FloatDataViewQuery;

template <typename T>
class DataViewQueryReference : public DataViewQuery<T> {
public:
	DataViewQueryReference(const DataViewQuery<T>& query) : query(query) {}
	virtual void updatePoints(const DataView<T>& view, std::vector<unsigned int>& points) const {
		query.updatePoints(view, points);
	}
 
private:
	const DataViewQuery<T>& query;
};

template <typename T>
class DataViewFilter : public DataViewQuery<T> {
public:
	void updatePoints(const DataView<T>& view, std::vector<unsigned int>& points) const {
		std::vector<unsigned int> newPoints;
		void* context = createContext(view);
		for (int f = 0; f < points.size(); f++) {
			if (isValid(view.getPoint(points[f]), context)) {
				newPoints.push_back(points[f]);
			}
		}

		deleteContext(context);

		points = newPoints;
	}

	virtual void* createContext(const DataView<T>& view) const { return NULL; }
	virtual void deleteContext(void* context) const {}
	virtual bool isValid(const T* point, void* context) const = 0;
};



template <typename T>
class DataViewSort : public DataViewQuery<T> {
public:
	struct CompareOperator : std::binary_function<unsigned int, unsigned int, bool>
	{
	    CompareOperator(const DataViewSort<T>* sort, const DataView<T>& view)
	    	: sort(sort), view(view) {}

	    bool operator()(unsigned int Lhs, unsigned int Rhs) const {
	    	return sort->compare(view, Lhs, Rhs);
	    }
 
 		const DataViewSort<T>* sort;
	    const DataView<T>& view;
	};

	void updatePoints(const DataView<T>& view, std::vector<unsigned int>& points) const {
		std::sort(points.begin(), points.end(), CompareOperator(this, view));
	}

	virtual bool compare(const DataView<T>& view, unsigned int Lhs, unsigned int Rhs) const = 0;
};

template <typename T>
class SortByDimension : public DataViewSort<T> {
public: 
	SortByDimension(unsigned int dimension) : dimension(dimension) {}

	bool compare(const DataView<T>& view, unsigned int Lhs, unsigned int Rhs) const {
	    return view.getPoint(Lhs)[dimension] < view.getPoint(Rhs)[dimension];
	}

private:
	unsigned int dimension;
};

/*

struct CompareFloatDataViewVariable : std::binary_function<unsigned int, unsigned int, bool>
{
    CompareFloatDataViewVariable(FloatDataView* data, int index, bool sortDesc)
    : data(data), index(index), sortDesc(sortDesc) {
    	numVariables = data->getVariables().size();
    }

    bool operator()(unsigned int Lhs, unsigned int Rhs)const
    {
    	bool compare = data->getArray()[numVariables*Lhs + index] < data->getArray()[numVariables*Rhs + index];
    	return sortDesc ? !compare : compare;
    }

    FloatDataView* data;
    int numVariables;
    int index;
    bool sortDesc;
};

void FloatDataRenderer::sortByVariable(int index, bool sortDesc) {
	if (index > 0) {

	    this->sortedIndices = fullIndices;
	    std::sort(this->sortedIndices.begin(), this->sortedIndices.end(), CompareFloatDataViewVariable(data, index, sortDesc));
	}
	else {
	    this->sortedIndices = fullIndices;
	}

	updateElementVersion++;
}*/

template <typename T>
class DimensionCompareFilter : public DataViewFilter<T> {
public:
	enum CompareType { LessThan, GreaterThan, LessThanEqual, GreaterThanEqual };

	DimensionCompareFilter(unsigned int dimension, const T& value, CompareType compareType) : dimension(dimension), value(value), compareType(compareType) {}

	bool isValid(const T* point, void* context) const {
		switch(compareType) {
			case LessThan:
				return point[dimension] < value;
				break;
			case GreaterThan:
				return point[dimension] > value;
				break;
			case LessThanEqual:
				return point[dimension] <= value;
				break;
			case GreaterThanEqual:
				return point[dimension] >= value;
				break;
		};

		return false;
	}

private:
	unsigned int dimension;
	T value;
	CompareType compareType;
};

template <typename T>
class TopKFilter : public DataViewFilter<T> {
public:
	TopKFilter(int k) : k(k) {}

	void* createContext(const DataView<T>& view) const { return new int(0); }
	void deleteContext(void* context) const { delete static_cast<int*>(context); }

	bool isValid(const T* point, void* context) const {
		int& curNum = *static_cast<int*>(context);
		if (curNum < k) {		
			curNum++;
			return true;
		}

		return false;
	}

private:
	int k;
};

}

#endif