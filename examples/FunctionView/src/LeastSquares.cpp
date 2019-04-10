#include "src/LeastSquares.h"

Eigen::VectorXf calculateLeastSquares(Eigen::MatrixXf& A, Eigen::VectorXf& b) {
	return A.bdcSvd(Eigen::ComputeThinU | Eigen::ComputeThinV).solve(b);;
}