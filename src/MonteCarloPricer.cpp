#include "MonteCarloPricer.hpp"
#include <cmath>
#include <random>

namespace pricing {
	double MonteCarloPricer::price( double spot, double riskFreeRate, double volatility, const Option& option, std::uint64_t simulations, std::uint64_t seed ) {
		const Payoff& payoff = option.getPayoff();
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

			// Delegate to the supplied Payoff so we stay agnostic to
			// whether this is a call, a put, or something more exotic.
			payoffSum += payoff( terminalPrice );
		}
		// Monte Carlo estimator: (1/N) · Σ payoff_i
		// This converges to E[max(S_T - K, 0)] by the Law of Large Numbers.
		const double averagePayoff = payoffSum / static_cast<double>( simulations );

		// Discount the expected payoff back to present value:
    	//   C = e^{-rT} · E[max(S_T - K, 0)]
		return std::exp( -riskFreeRate * maturity ) * averagePayoff;
	}
} // namespace pricing
