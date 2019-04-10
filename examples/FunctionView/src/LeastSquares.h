#ifndef LEAST_SQUARES_H_
#define LEAST_SQUARES_H_

#include <iostream>
#include <Eigen/Dense>

Eigen::VectorXf calculateLeastSquares(Eigen::MatrixXf& A, Eigen::VectorXf& b);

#endif
