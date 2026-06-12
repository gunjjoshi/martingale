#pragma once

#include <cstdint>
#include "Option.hpp"

namespace pricing {

	class MonteCarloPricer {
		public:
			// Prices a European option using naive Monte Carlo.
			//
			// The option's payoff (call, put, ...) is supplied through the
			// Option's Payoff, so this one method prices every instrument whose
			// value depends only on the terminal price S_T.
			//
			// Parameters:
			//   spot         — current stock price S₀
			//   riskFreeRate — continuously compounded risk-free rate r
			//   volatility   — annualised vol σ
			//   option       — product descriptor (holds the payoff and maturity)
			//   simulations  — number of paths N
			//   seed         — RNG seed.
			//
			// Returns the estimated present value: e^{-rT} · E[ payoff(S_T) ]
			static double price( double spot, double riskFreeRate, double volatility, const Option& option, std::uint64_t simulations, std::uint64_t seed = 0 );
	};
} // namepsace pricing

