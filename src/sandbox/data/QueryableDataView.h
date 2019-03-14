#ifndef SANDBOX_QUERYABLE_DATA_VIEW_H_
#define SANDBOX_QUERYABLE_DATA_VIEW_H_

#include "sandbox/data/DataViewDecorator.h"
#include "sandbox/SceneNode.h"
#include <algorithm>

namespace sandbox {

template <typename T>
class DataViewQuery {
public:
	virtual bool updateArray(const DataView<T>& view, std::vector<T>& array) const { return false; }
	virtual bool updatePoints(const DataView<T>& view, std::vector<unsigned int>& points) const { return false; }
};

template <typename T>
class QueryableDataView : public DataViewDecorator<T> {
public:
	QueryableDataView(SceneNode* node) : DataViewDecorator<T>(node), pointsChanged(false) {
		SceneComponent::addType< QueryableDataView<T> >();
	}

	void updateModel() {
		DataViewDecorator<T>::updateModel();
		const DataView<T>* view = DataViewDecorator<T>::getView();
		if (view && version != this->getVersion()) {
			points.clear();
			array.clear();
			arrayChanged = false;
			pointsChanged = false;

			for (int f = 0; f < queries.size(); f++) {
				bool changed = queries[f]->updateArray(*this, array);
				arrayChanged = changed || arrayChanged; 
				if (changed) {
					statistics = this->calculateStatistics();
				}
				pointsChanged = queries[f]->updatePoints(*this, points) || pointsChanged;
			}

			version = this->getVersion();
		}
	}

	virtual ~QueryableDataView() {
		for (int f = 0; f < queries.size(); f++) {
			delete queries[f];
		}
	}

	void addQuery(DataViewQuery<T>* query) {
		queries.push_back(query);
		this->updateVersion();
	}

	virtual const std::vector<T>& getArray() const {
		if (arrayChanged) {
			return array; 
		}

		return DataViewDecorator<T>::getArray();
	}

	const std::vector<unsigned int>& getPoints() const { 
		if (pointsChanged) {
			return points; 
		}

		return DataViewDecorator<T>::getPoints();
	}

	const typename DataView<T>::DataViewStatistics& getStatistics(unsigned int dimension) const {
		if (arrayChanged) {
			return statistics[dimension]; 
		}

		return DataViewDecorator<T>::getStatistics(dimension);
	}

private:
	std::vector<DataViewQuery<T>*> queries;
	long version;
	std::vector<T> array;
	std::vector<unsigned int> points;
	bool pointsChanged;
	bool arrayChanged;
	std::vector<typename DataView<T>::DataViewStatistics> statistics;
};

typedef QueryableDataView<float> FloatQueryableDataView;
typedef DataViewQuery<float> FloatDataViewQuery;

template <typename T>
class DataViewQueryReference : public DataViewQuery<T> {
public:
	DataViewQueryReference(const DataViewQuery<T>& query) : query(query) {}
	virtual bool updatePoints(const DataView<T>& view, std::vector<unsigned int>& points) const {
		query.updatePoints(view, points);
		return true;
	}
 
private:
	const DataViewQuery<T>& query;
};

template <typename T>
class DataViewFilter : public DataViewQuery<T> {
public:
	bool updatePoints(const DataView<T>& view, std::vector<unsigned int>& points) const {
		std::vector<unsigned int> newPoints;
		const std::vector<unsigned int>& oldPoints = view.getPoints();
		void* context = createContext(view);
		for (int f = 0; f < oldPoints.size(); f++) {
			if (isValid(view.getPoint(oldPoints[f]), context)) {
				newPoints.push_back(oldPoints[f]);
			}
		}

		deleteContext(context);

		points = newPoints;
		return true;
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

	bool updatePoints(const DataView<T>& view, std::vector<unsigned int>& points) const {
		points = view.getPoints();
		std::sort(points.begin(), points.end(), CompareOperator(this, view));

		return true;
	}

	virtual bool compare(const DataView<T>& view, unsigned int Lhs, unsigned int Rhs) const = 0;
};

template <typename T>
class DataViewApply : public DataViewQuery<T> {
public:
	bool updateArray(const DataView<T>& view, std::vector<T>& array) const {
		array = view.getArray();
		int numVariables = view.getVariables().size();
		for (int f = 0; f < array.size()/numVariables; f++) {
			apply(&array[f*numVariables]);
		}

		return true;
	}

	virtual void apply(T* point) const = 0;
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

template <typename T>
class ApplyLogScale : public DataViewApply<T> {
public:
	ApplyLogScale() : dimensions() {}
	ApplyLogScale(std::vector<unsigned int> dimensions) : dimensions(dimensions) {}
	ApplyLogScale(unsigned int dimension) : dimensions() {
		addDimension(dimension);
	}

	void addDimension(unsigned int dimension) {
		dimensions.push_back(dimension);
	}

	virtual void apply(T* point) const {
		for (int f = 0; f < dimensions.size(); f++) {
			unsigned int dimension = dimensions[f];
			point[dimension] = std::log(point[dimension]);
		}
	}

private:
	std::vector<unsigned int> dimensions;
};

}

#endif