#ifndef SANDBOX_DATA_VIEW_H_
#define SANDBOX_DATA_VIEW_H_

#include "sandbox/data/KdTree.h"

namespace sandbox {

template <typename T>
class DataView : public KdSearchable<T> {
public:
	virtual ~DataView() {}
	virtual const std::vector<std::string>& getVariables() const = 0;
	virtual const std::vector<T>& getArray() const = 0;
	virtual const std::vector<unsigned int>& getPoints() const = 0;
	virtual T getMin(unsigned int dimension) const = 0;
	virtual T getMax(unsigned int dimension) const = 0;
};

}

#endif