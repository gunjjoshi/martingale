#pragma once

#include "types.hpp"

namespace martingale::core::linalg {

/**
 * @brief Compute Cholesky decomposition of symmetric positive-definite matrix
 * 
 * Used for generating correlated random variables from correlation matrix.
 * 
 * The Cholesky decomposition of a Hermitian positive-definite matrix A is a
 * decomposition of the form:
 * 
 * A = L L*
 * 
 * where L is a lower triangular matrix with real and positive diagonal entries,
 * and L* denotes the conjugate transpose of L. Every Hermitian positive-definite
 * matrix (and thus also every real symmetric positive-definite matrix) has a
 * Cholesky decomposition and the lower triangular matrix is unique if we impose
 * the diagonal to be strictly positive.
 */
inline Matrix cholesky(const ConstMatrixRef& A) {
    Eigen::LLT<Eigen::MatrixXd> llt(A); // Cholesky decomposition object from matrix A
    return llt.matrixL(); // extract the lower triangular matrix
}

/**
 * @brief Compute covariance matrix from returns
 * 
 * Computes the covariance matrix from historical return data
 */
inline Matrix covariance(const ConstMatrixRef& returns) {
    // (rows = time periods, columns = assets)
    const auto centered = returns.rowwise() - returns.colwise().mean();
    return (centered.transpose() * centered) / (returns.rows() - 1);
}

/**
 * @brief Compute correlation matrix from covariance matrix
 * 
 * * If @f$X@f$ and @f$Y@f$ are two random variables, with means (expected
 * values) @f$\mu_X@f$ and @f$\mu_Y@f$ and standard deviations @f$\sigma_X@f$
 * and @f$\sigma_Y@f$, respectively, then their covariance and correlation
 * are defined as follows:
 *
 * @b Covariance:
 * @f[
 *   \mathrm{cov}_{XY} = \sigma_{XY} = E\big[(X - \mu_X)(Y - \mu_Y)\big]
 * @f]
 *
 * @b Correlation:
 * @f[
 *   \mathrm{corr}_{XY} = \rho_{XY} =
 *     \frac{E\big[(X - \mu_X)(Y - \mu_Y)\big]}{\sigma_X \sigma_Y}
 * @f]
 *
 * so that:
 * @f[
 *   \rho_{XY} = \frac{\sigma_{XY}}{\sigma_X \sigma_Y}
 * @f]
 *
 * where @f$E@f$ is the expected value operator.
 */
inline Matrix correlation_from_covariance(const ConstMatrixRef& cov) {
    Vector std_devs = cov.diagonal().array().sqrt(); // square root of diagonal elements [σ₁, σ₂, σ₃]
    Matrix corr = cov;
    corr = corr.array().colwise() / std_devs.array();
    corr = corr.array().rowwise() / std_devs.transpose().array();
    return corr;
}

} // namespace martingale::core::linalg