#ifndef SANDBOX_ADVANCED_DATA_VIEW_H_
#define SANDBOX_ADVANCED_DATA_VIEW_H_

#include "sandbox/data/DataViewDecorator.h"
#include "sandbox/SceneNode.h"

namespace sandbox {

template <typename T>
class DataViewFilter {
public:
	virtual void* createContext(const DataView<T>& view) { return NULL; }
	virtual void deleteContext(void* context) {}
	virtual bool isValid(const T* point, void* context) const = 0;
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
			points.clear();

			std::vector<void*> filterContexts;
			for (int f = 0; f < filters.size(); f++) {
				filterContexts.push_back(filters[f]->createContext(*view));
			}

			const std::vector<unsigned int>& oldPoints = view->getPoints();
			for (int f = 0; f < oldPoints.size(); f++) {
				unsigned int index = oldPoints[f];
				const T* point = view->getPoint(index);
				bool isValid = true;

				for (int i = 0; i < filters.size(); i++) {
					isValid = filters[i]->isValid(point, filterContexts[i]);
					if (!isValid) {
						break;
					}
				}

				if (isValid) {
					points.push_back(index);
				}
			}

			for (int f = 0; f < filters.size(); f++) {
				filters[f]->deleteContext(filterContexts[f]);
			}

			version = SceneComponent::getVersion();
		}
	}

	virtual ~AdvancedDataView() {
		for (int f = 0; f < filters.size(); f++) {
			delete filters[f];
		}
	}

	void addFilter(DataViewFilter<T>* filter) {
		filters.push_back(filter);
		SceneComponent::updateVersion();
	}

	const std::vector<unsigned int>& getPoints() const { return points; }

private:
	std::vector<DataViewFilter<T>*> filters;
	long version;
	std::vector<unsigned int> points;
};

typedef AdvancedDataView<float> FloatAdvancedDataView;
typedef DataViewFilter<float> FloatDataViewFilter;

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

	void* createContext(const DataView<T>& view) { return new int(0); }
	void deleteContext(void* context) { delete static_cast<int*>(context); }

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