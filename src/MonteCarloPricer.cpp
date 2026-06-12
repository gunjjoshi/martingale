#include "MonteCarloPricer.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace pricing {
	double MonteCarloPricer::priceEuropeanCall( double spot, double riskFreeRate, double volatility, const Option& option, std::uint64_t simulations, std::uint64_t seed ) {
		const double strike = option.getStrike();
		const double maturity = option.getMaturity();

		std::mt19937_64 generator( seed != 0 ? seed : std::random_device{}());

		// This corresponds to the 'Z' in the GBM formula:
    	//   S_T = S₀ · exp((r - σ²/2)·T + σ·√T·Z)
		std::normal_distribution<double> normal( 0.0, 1.0 );

		// The deterministic part of the GBM exponent: (r - σ²/2) · T
		const double drift = ( riskFreeRate - 0.5*volatility*volatility ) * maturity;

		// The stochastic scaling factor: σ · √T
		const double diffusion = volatility * std::sqrt( maturity );

		double payoffSum = 0.0;
		for ( std::uint64_t i = 0; i < simulations; i++ ) {
			const double z = normal( generator );

			// Apply the closed-form GBM terminal price formula:
			//   S_T = S₀ · exp((r - σ²/2)·T   +     σ·√T·Z)
			//              └────── drift ───┘   └─diffusion·z─┘
			const double terminalPrice = spot * std::exp( drift + diffusion*z );

			// European call payoff: max(S_T - K, 0)
			//   If S_T > K the option is in-the-money; payoff = S_T - K.
        	//   If S_T ≤ K the option expires worthless; payoff = 0.
			payoffSum += std::max( terminalPrice - strike, 0.0 );
		}
		// Monte Carlo estimator: (1/N) · Σ payoff_i
		// This converges to E[max(S_T - K, 0)] by the Law of Large Numbers.
		const double averagePayoff = payoffSum / static_cast<double>( simulations );

		// Discount the expected payoff back to present value:
    	//   C = e^{-rT} · E[max(S_T - K, 0)]
		return std::exp( -riskFreeRate * maturity ) * averagePayoff;
	}

	double MonteCarloPricer::priceEuropeanCall( const PricingRequest& req, std::uint64_t seed ) {
		Option option( req.strike, req.maturity);

		return priceEuropeanCall( req.spot, req.riskFreeRate, req.volatility, option, req.simulations, seed );
	}
} // namespace pricing
