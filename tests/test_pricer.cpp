#include <cmath>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include "MonteCarloPricer.hpp"
#include "Payoff.hpp"
#include "PathPayoff.hpp"
#include "PathOption.hpp"
using namespace pricing;

// Computes Φ(x), the cumulative distribution function of N(0,1).
// erfc(y) = 1 - erf(y) = 2·Φ(-y·√2).
static double normCdf( double x ) {
	return 0.5 * std::erfc( -x / std::sqrt( 2.0 ) ); // 0.5*(1 + erf(x/√2))
}

static double blackScholesCall( double s, double k, double r, double sigma, double t ) {
	// d₁ = [ln(S₀/K) + (r + σ²/2)·T] / (σ·√T)
	// The numerator is the log-moneyness adjusted for the expected drift.
	// The denominator normalises by total volatility over the life of the option.
	const double d1 = ( std::log( s/k ) + ( r + 0.5*sigma*sigma )*t ) / ( sigma*std::sqrt(t) );

	// d₂ = d₁ - σ·√T
	// Represents the risk-neutral probability (in standard normal space) that the option expires in-the-money.
	const double d2 = d1 - ( sigma * std::sqrt(t) );

	// Black-Scholes formula: C = S₀·Φ(d₁) - K·e^{-rT}·Φ(d₂)
	//   S₀·Φ(d₁)        — delta-weighted current stock price (expected receipt)
	//   K·e^{-rT}·Φ(d₂) — PV of strike payment, weighted by exercise probability
	return s * normCdf( d1 ) - ( k * std::exp( -r*t ) * normCdf( d2 ) );
}

static double blackScholesPut( double s, double k, double r, double sigma, double t ) {
	const double d1 = ( std::log( s/k ) + ( r + 0.5*sigma*sigma )*t ) / ( sigma*std::sqrt(t) );
	const double d2 = d1 - ( sigma * std::sqrt(t) );

	// P = K·e^{-rT}·Φ(-d₂) - S₀·Φ(-d₁)
	return ( k * std::exp( -r*t ) * normCdf( -d2 ) ) - s * normCdf( -d1 );
}

// Because the geometric average of M lognormals is itself lognormal, the
// problem reduces to a Black-Scholes formula with adjusted parameters:
//
//   σ_A² = σ² · (M+1)(2M+1) / (6M²)       [annualised variance of log-average]
//   μ_A  = (r - σ²/2) · (M+1) / (2M)       [annualised drift of log-average]
//
// These arise from summing the variances and covariances of the Brownian increments
// W_{t_1}, ..., W_{t_M} where t_i = i·T/M.
//
// Reference: Kemna & Vorst (1990), "A Pricing Method for Options Based on Average
// Asset Values", Journal of Banking and Finance.
static double geoAsianCall( double s, double k, double r, double sigma, double t, std::uint64_t M ) {
	const double Md = static_cast<double>( M );

	// σ_A² = σ² · (M+1)(2M+1) / (6M²)
	const double sigmaA2 = sigma * sigma * ( Md + 1.0 ) * ( ( 2.0 * Md ) + 1.0 ) / ( 6.0 * Md * Md );
	const double sigmaA  = std::sqrt( sigmaA2 );

	// μ_A  = (r - σ²/2) · (M+1) / (2M)
	const double muA = ( r - ( 0.5 * sigma * sigma ) ) * ( Md + 1.0 ) / ( 2.0 * Md );

	const double d1 = ( std::log( s/k ) + ( muA + 0.5 * sigmaA2 ) * t ) / ( sigmaA * std::sqrt(t) );
	const double d2 = d1 - ( sigmaA * std::sqrt( t ) );

	// The geometric average is lognormal with:
	//   E[ln G] = ln(S₀) + μ_A·T
	//   Var[ln G] = σ_A²·T
	// So we apply the Black-Scholes formula but replace S₀ with the geometric
	// average forward S₀·exp((μ_A + σ_A²/2)·T) and discount at rate r.
	return std::exp( -r*t ) * ( s * std::exp( ( muA + 0.5*sigmaA2 )*t ) * normCdf( d1 ) - k * normCdf( d2 ) );
}

// DOC = A - B  where:
//   A = vanilla Black-Scholes call
//   B = (H/S)^{2λ} correction term encoding knock-out probability via the
//       reflection principle applied to the Brownian motion
//
//   λ = (r + σ²/2) / σ²   [dimension-free drift parameter]
//   y₁ = [ln(H²/(S·K)) + (r + σ²/2)·T] / (σ·√T)
//   y₂ = y₁ - σ·√T
//
// Note: the MC test uses discrete monitoring (M=252 steps). There is a small
// discrepancy vs this continuous formula — the Broadie-Glasserman-Kou (1997)
// continuity correction handles it, but is not applied here. With M=252, the
// gap is small enough that a tolerance of 0.25 comfortably covers it.
//
// Reference: Rubinstein & Reiner (1991), "Breaking Down the Barriers",
//            Risk Magazine, September 1991.
static double downAndOutCall( double s, double k, double h, double r, double sigma, double t ) {
	const double lam = ( r + 0.5*sigma*sigma ) / ( sigma*sigma );

	const double d1 = ( std::log( s/k ) + ( r + 0.5*sigma*sigma )*t ) / ( sigma*std::sqrt(t) );
	const double d2 = d1 - sigma*std::sqrt( t );

	// "Reflected" quantities — evaluated at the image spot H²/S.
	const double y1 = ( std::log( h*h / (s*k) ) + ( r + 0.5*sigma*sigma )*t ) / ( sigma*std::sqrt(t) );
	const double y2 = y1 - sigma*std::sqrt( t );

	// A = vanilla call price
	const double A = s * normCdf( d1 ) - k * std::exp( -r*t ) * normCdf( d2 );

	// B = knock-out correction
	const double B = s * std::pow( h/s, 2.0*lam ) * normCdf( y1 ) - k * std::exp( -r*t ) * std::pow( h/s, 2.0*lam - 2.0 ) * normCdf( y2 );

	return A - B;
}

static bool check( const char* label, double analytic, double mc, double tolerance ) {
	const double err = std::abs( mc - analytic );
	const bool   ok  = err <= tolerance;
	std::cout << std::fixed << std::setprecision( 4 )
			  << label << "\n"
			  << "  Analytic : " << analytic << "\n"
			  << "  MC       : " << mc << "\n"
			  << "  Abs err  : " << err  << "  (tol " << tolerance << ")\n"
			  << "  " << ( ok ? "PASS" : "FAIL" ) << "\n\n";
	return ok;
}

int main() {
	const double s = 100.0, k = 100.0, r = 0.05, sigma = 0.20, t = 1.0;
	const double barrier = 80.0;
	const std::uint64_t M = 252;

	bool allPassed = true;

	// European call & put
	{
		CallPayoff call( k );
		PutPayoff  put( k );
		Option callOpt( call, t );
		Option putOpt( put, t );

		const double mcCall = MonteCarloPricer::price( s, r, sigma, callOpt, 1'000'000, 42 );
		const double mcPut  = MonteCarloPricer::price( s, r, sigma, putOpt,  1'000'000, 42 );

		allPassed &= check( "European Call", blackScholesCall( s, k, r, sigma, t ), mcCall, 0.05 );
		allPassed &= check( "European Put",  blackScholesPut(  s, k, r, sigma, t ), mcPut,  0.05 );
	}

	// Geometric Asian call
	{
		GeometricAsianCallPayoff geoCall( k );
		PathOption geoOpt( geoCall, t, M );

		const double mcGeoAsian = MonteCarloPricer::pricePath( s, r, sigma, geoOpt, 1'000'000, 42 );

		allPassed &= check( "Geometric Asian Call (M=252)", geoAsianCall( s, k, r, sigma, t, M ), mcGeoAsian, 0.05 );
	}

	// Down-and-Out call
	{
		DownAndOutCallPayoff doc( k, barrier );
		PathOption docOpt( doc, t, M );

		const double mcDOC = MonteCarloPricer::pricePath( s, r, sigma, docOpt, 1000000, 42 );
		const double analyticDOC = downAndOutCall( s, k, barrier, r, sigma, t );

		allPassed &= check( "Down-and-Out Call ( B = 80, M = 252, continuous formula )", analyticDOC, mcDOC, 0.25 );

		// DOC must be cheaper than the vanilla call
		const double vanillaCall = blackScholesCall( s, k, r, sigma, t );
		const bool sane = analyticDOC < vanillaCall;

		std::cout << "  Sanity (DOC < vanilla call): " << analyticDOC
				  << " < " << vanillaCall << " => " << ( sane ? "PASS" : "FAIL" ) << "\n\n";
		allPassed &= sane;
	}

	if ( !allPassed ) {
		std::cerr << "One or more tests FAILED.\n";
		return EXIT_FAILURE;
	}

	std::cout << "All tests PASSED.\n";
	return EXIT_SUCCESS;
}
