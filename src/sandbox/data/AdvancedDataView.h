#ifndef SANDBOX_ADVANCED_DATA_VIEW_H_
#define SANDBOX_ADVANCED_DATA_VIEW_H_

#include "sandbox/data/DataViewDecorator.h"
#include "sandbox/SceneNode.h"

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