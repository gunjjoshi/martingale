#pragma once

#include <cstdint>
#include "Option.hpp"

namespace pricing {
	struct PricingRequest {
		double spot;
		double riskFreeRate;
		double volatility;
		double maturity;
		double strike;
		std::uint64_t simulations;
	};

	class MonteCarloPricer {
		public:
			// Prices a European call option using naive Monte Carlo.
			//
			// Parameters:
			//   spot         — current stock price S₀
			//   riskFreeRate — continuously compounded risk-free rate r
			//   volatility   — annualised vol σ
			//   option       — product descriptor (holds strike and maturity)
			//   simulations  — number of paths N
			//   seed         — RNG seed. 0 means "use std::random_device" (non-deterministic).
			//                 Non-zero seeds make the output deterministic, which is
			//                 essential for reproducible unit tests.
			//
			// Returns the estimated present value of the call: e^{-rT} · E[max(S_T - K, 0)]
			static double priceEuropeanCall( double spot, double riskFreeRate, double volatility, const Option& option, std::uint64_t simulations, std::uint64_t seed = 0 );

			static double priceEuropeanCall( const PricingRequest& req, std::uint64_t seed = 0 );
	};
} // namepsace pricing

