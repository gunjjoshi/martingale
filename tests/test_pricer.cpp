#include <cmath>
#include <cstdlib>
#include <iostream>
#include "MonteCarloPricer.hpp"
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

int main() {
	const double s = 100.0, k = 100.0, r = 0.05, sigma = 0.20, t = 1.0;
	Option option(k, t);

	const double analytic = blackScholesCall(s, k, r, sigma, t);
	const double mc = MonteCarloPricer::priceEuropeanCall( s, r, sigma, option, 1'000'000, /*seed=*/42 );
	const double absErr = std::abs(mc - analytic);

	std::cout << "Black-Scholes: " << analytic << "\n" << "Monte Carlo  : " << mc       << "\n" << "Abs error    : " << absErr   << "\n";

	if ( absErr > 0.05 ) {
		std::cerr << "FAIL: MC price deviates too much from Black-Scholes.\n";
		return EXIT_FAILURE;
	}

	std::cout << "PASS\n";
	return EXIT_SUCCESS;
}
