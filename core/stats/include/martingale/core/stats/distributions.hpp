#pragma once

#include <cmath>
#include <limits>

// Optional: Use Boost for more accurate implementations
#ifdef MARTINGALE_WITH_BOOST
    #include <boost/math/distributions/normal.hpp>
#endif

namespace martingale::core::stats {

/**
 * @brief Standard normal cumulative distribution function
 * 
 * Computes @f$\Phi(x)@f$, the cumulative distribution function (CDF) of a
 * standard normal random variable @f$Z \sim N(0, 1)@f$. This gives the
 * probability that @f$Z@f$ is less than or equal to a given value @f$x@f$,
 * i.e. the area under the standard normal density from @f$-\infty@f$ to @f$x@f$:
 *
 * @f[
 *   \Phi(x) = P(Z \le x) = \frac{1}{\sqrt{2\pi}} \int_{-\infty}^{x} e^{-t^2/2}\, dt
 * @f]
 *
 * @b Relation @b to @b the @b error @b function:
 *
 * @f$\Phi(x)@f$ can be expressed in terms of the error function
 * @f$\mathrm{erf}@f$ as:
 *
 * @f[
 *   \Phi(x) = \frac{1}{2}\left[1 + \mathrm{erf}\left(\frac{x}{\sqrt{2}}\right)\right]
 * @f]
 *
 * When Boost is unavailable, @f$\mathrm{erf}@f$ is evaluated using the
 * Abramowitz and Stegun rational approximation:
 *
 * @f[
 *   \mathrm{erf}(x) \approx 1 - \left(a_1 t + a_2 t^2 + a_3 t^3 + a_4 t^4 + a_5 t^5\right) e^{-x^2},
 *   \qquad t = \frac{1}{1 + p x}
 * @f]
 *
 * with coefficients:
 * @f[
 *   p = 0.3275911, \quad
 *   a_1 = 0.254829592, \quad
 *   a_2 = -0.284496736, \quad
 *   a_3 = 1.421413741, \quad
 *   a_4 = -1.453152027, \quad
 *   a_5 = 1.061405429
 * @f]
 *
 * For negative @f$x@f$, the symmetry @f$\mathrm{erf}(-x) = -\mathrm{erf}(x)@f$
 * is used (handled via the @c sign flip in the implementation), and the final
 * result is assembled from @f$\Phi(x) = \tfrac{1}{2}\left[1 + \mathrm{sign}(x)\,\mathrm{erf}\!\left(|x|/\sqrt{2}\right)\right]@f$.
 *
 * @param x Value at which to evaluate the CDF.
 * @return Probability @f$\Phi(x) \in (0, 1)@f$.
 *
 * @note Uses Boost's @c boost::math::cdf when @c MARTINGALE_WITH_BOOST is
 *       defined, for full double-precision accuracy. Otherwise falls back
 *       to the Abramowitz-Stegun approximation above.
 */
inline double normal_cdf(double x) {
#ifdef MARTINGALE_WITH_BOOST
    boost::math::normal_distribution<> normal;
    return boost::math::cdf(normal, x);
#else
    // Abramowitz and Stegun approximation:
    const double a1 =  0.254829592;
    const double a2 = -0.284496736;
    const double a3 =  1.421413741;
    const double a4 = -1.453152027;
    const double a5 =  1.061405429;
    const double p  =  0.3275911;
    
    int sign = (x < 0) ? -1 : 1;
    x = std::abs(x) / std::sqrt(2.0);
    
    // t = 1 / (1 + px):
    double t = 1.0 / (1.0 + p * x);

    // erf(x) ≈ 1 - (a₁t + a₂t² + a₃t³ + a₄t⁴ + a₅t⁵)e^{-x²}:
    double y = 1.0 - (((((a5 * t + a4) * t) + a3) * t + a2) * t + a1) * t * std::exp(-x * x);
    
    // Φ(x) = (1/2)[1 + erf(x/√2)]:
    return 0.5 * (1.0 + sign * y);
#endif
}

/**
 * @brief Inverse of the standard normal CDF (quantile function).
 *
 * Computes @f$\Phi^{-1}(p)@f$, the quantile function of a standard normal
 * random variable @f$Z \sim N(0, 1)@f$. Given a probability @f$p \in (0, 1)@f$,
 * it returns the value @f$x@f$ such that:
 *
 * @f[
 *   \Phi(x) = p \quad \Longleftrightarrow \quad x = \Phi^{-1}(p)
 * @f]
 *
 * where @f$\Phi@f$ is the standard normal CDF (see @c normal_cdf). This is the
 * value at which the cumulative probability under the standard normal curve
 * reaches @f$p@f$.
 *
 * When Boost is unavailable, @f$\Phi^{-1}(p)@f$ is evaluated using
 * Peter Acklam's rational approximation algorithm, which splits the
 * domain of @f$p@f$ into three regions and fits a separate rational
 * function to each:
 *
 * - @b Lower @b region (@f$0 < p < p_{\text{low}}@f$), using
 *   @f$q = \sqrt{-2 \ln p}@f$:
 *   @f[
 *     x = \frac{((((c_1 q + c_2)q + c_3)q + c_4)q + c_5)q + c_6}
 *              {(((d_1 q + d_2)q + d_3)q + d_4)q + 1}
 *   @f]
 *
 * - @b Central @b region (@f$p_{\text{low}} \le p \le p_{\text{high}}@f$),
 *   using @f$q = p - 0.5@f$ and @f$r = q^2@f$:
 *   @f[
 *     x = \frac{(((((a_1 r + a_2)r + a_3)r + a_4)r + a_5)r + a_6)\,q}
 *              {((((b_1 r + b_2)r + b_3)r + b_4)r + b_5)r + 1}
 *   @f]
 *
 * - @b Upper @b region (@f$p_{\text{high}} < p < 1@f$), using
 *   @f$q = \sqrt{-2 \ln(1 - p)}@f$:
 *   @f[
 *     x = -\frac{((((c_1 q + c_2)q + c_3)q + c_4)q + c_5)q + c_6}
 *               {(((d_1 q + d_2)q + d_3)q + d_4)q + 1}
 *   @f]
 *
 * with break-points @f$p_{\text{low}} = 0.02425@f$ and
 * @f$p_{\text{high}} = 1 - p_{\text{low}}@f$, and fixed rational-approximation
 * coefficients @f$a_1, \dots, a_6@f$, @f$b_1, \dots, b_5@f$,
 * @f$c_1, \dots, c_6@f$, and @f$d_1, \dots, d_4@f$.
 *
 * @param p Probability at which to evaluate the quantile function, expected
 *          in the open interval @f$(0, 1)@f$.
 * @return The value @f$x@f$ such that @f$\Phi(x) = p@f$.
 *
 * @note Uses Boost's @c boost::math::quantile when @c MARTINGALE_WITH_BOOST
 *       is defined, for full double-precision accuracy. Otherwise falls
 *       back to Peter Acklam's algorithm above.
 */
inline double normal_inv_cdf(double p) {
#ifdef MARTINGALE_WITH_BOOST
    boost::math::normal_distribution<> normal;
    return boost::math::quantile(normal, p);
#else
    // Peter Acklam's algorithm:
    if (p <= 0.0 || p >= 1.0) {
        if (p == 0.0) return -std::numeric_limits<double>::infinity();
        if (p == 1.0) return std::numeric_limits<double>::infinity();
        return std::numeric_limits<double>::quiet_NaN();
    }

    // Coefficients in rational approximations
    const double a1 = -3.969683028665376e+01;
    const double a2 =  2.209460984245205e+02;
    const double a3 = -2.759285104469687e+02;
    const double a4 =  1.383577518672690e+02;
    const double a5 = -3.066479806614716e+01;
    const double a6 =  2.506628277459239e+00;

    const double b1 = -5.447609879822406e+01;
    const double b2 =  1.615858368580409e+02;
    const double b3 = -1.556989798598866e+02;
    const double b4 =  6.680131188771972e+01;
    const double b5 = -1.328068155288572e+01;

    const double c1 = -7.784894002430293e-03;
    const double c2 = -3.223964580411365e-01;
    const double c3 = -2.400758277161838e+00;
    const double c4 = -2.549732539343734e+00;
    const double c5 =  4.374664141464968e+00;
    const double c6 =  2.938163982698783e+00;

    const double d1 =  7.784695709041462e-03;
    const double d2 =  3.224671290700398e-01;
    const double d3 =  2.445134137142996e+00;
    const double d4 =  3.754408661907416e+00;

    const double p_low  = 0.02425;
    const double p_high = 1.0 - p_low;

    double q, r, x;

    if (p < p_low) {
        //   Lower Tail (p < 0.02425):
        //   q = √(-2 ln(p))
        //          c₀ + c₁q + c₂q² + c₃q³ + c₄q⁴ + c₅q⁵
        //   x = ───────────────────────────────────────
        //          1 + d₁q + d₂q² + d₃q³ + d₄q⁴

        q = std::sqrt(-2.0 * std::log(p));
        x = (((((c1*q + c2)*q + c3)*q + c4)*q + c5)*q + c6) /
            ((((d1*q + d2)*q + d3)*q + d4)*q + 1.0);
    } else if (p <= p_high) {
        //   Central (0.02425 ≤ p ≤ 0.97575):
        //   q = p - 0.5,  r = q²
        //          (a₁r + a₂r² + a₃r³ + a₄r⁴ + a₅r⁵ + a₆) × q
        //   x = ───────────────────────────────────────────────
        //          1 + b₁r + b₂r² + b₃r³ + b₄r⁴ + b₅r⁵

        q = p - 0.5;
        r = q * q;
        x = (((((a1*r + a2)*r + a3)*r + a4)*r + a5)*r + a6)*q /
            (((((b1*r + b2)*r + b3)*r + b4)*r + b5)*r + 1.0);
    } else {
        //   Upper Tail (p > 0.97575):
        //   q = √(-2 ln(1 - p))
        //           c₀ + c₁q + c₂q² + c₃q³ + c₄q⁴ + c₅q⁵
        //   x = - ───────────────────────────────────────
        //           1 + d₁q + d₂q² + d₃q³ + d₄q⁴

        q = std::sqrt(-2.0 * std::log(1.0 - p));
        x = -(((((c1*q + c2)*q + c3)*q + c4)*q + c5)*q + c6) /
             ((((d1*q + d2)*q + d3)*q + d4)*q + 1.0);
    }

    return x;
#endif
}

} // namespace martingale::core::stats