#pragma once

#include <Eigen/Dense>

namespace martingale::core::linalg {

// Type aliases:
// Vector = Column vector
// X = Dynamic size (determined at runtime)
// d = double precision
using Vector = Eigen::VectorXd;
using Matrix = Eigen::MatrixXd; // Matrix = 2D matrix

using VectorRef = Eigen::Ref<Vector>;
using MatrixRef = Eigen::Ref<Matrix>;

using ConstVectorRef = Eigen::Ref<const Vector>;
using ConstMatrixRef = Eigen::Ref<const Matrix>;

} // namespace martingale::core::linalg