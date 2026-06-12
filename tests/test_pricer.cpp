#include <cmath>
#include <cstdlib>
#include <iostream>
#include "MonteCarloPricer.hpp"
#include "Payoff.hpp"
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

int main() {
	const double s = 100.0, k = 100.0, r = 0.05, sigma = 0.20, t = 1.0;
	const double tolerance = 0.05;

	CallPayoff call( k );
	PutPayoff put( k );
	Option callOption( call, t );
	Option putOption( put, t );

	const double analyticCall = blackScholesCall( s, k, r, sigma, t );
	const double analyticPut = blackScholesPut( s, k, r, sigma, t );
	const double mcCall = MonteCarloPricer::price( s, r, sigma, callOption, 1'000'000, /*seed=*/42 );
	const double mcPut = MonteCarloPricer::price( s, r, sigma, putOption, 1'000'000, /*seed=*/42 );
	const double callErr = std::abs( mcCall - analyticCall );
	const double putErr = std::abs( mcPut - analyticPut );

	std::cout << "Call  Black-Scholes: " << analyticCall << "\n"
	          << "Call  Monte Carlo  : " << mcCall << "\n"
	          << "Call  Abs error    : " << callErr << "\n\n"
	          << "Put   Black-Scholes: " << analyticPut << "\n"
	          << "Put   Monte Carlo  : " << mcPut << "\n"
	          << "Put   Abs error    : " << putErr << "\n";

	if ( callErr > tolerance || putErr > tolerance ) {
		std::cerr << "FAIL: MC price deviates too much from Black-Scholes.\n";
		return EXIT_FAILURE;
	}

	std::cout << "PASS\n";
	return EXIT_SUCCESS;
}
