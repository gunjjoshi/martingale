#pragma once

namespace pricing {

	// Maps a terminal asset price S_T to the option's payoff.
	// Any instrument whose value depends only on the price at maturity can be
	// expressed as a concrete Payoff. This is the abstraction that lets a single
	// Monte Carlo engine price calls, puts, and more exotic profiles
	// without ever knowing which one it is dealing with.
	class Payoff {

		public:
			virtual ~Payoff() = default;

			// Returns the payoff for a given asset price at maturity S_T.
			virtual double operator()( double spotAtMaturity ) const = 0;
	};

	// European call payoff: max(S_T - K, 0).
	class CallPayoff : public Payoff {

		public:
			explicit CallPayoff( double strike );
			double operator()( double spotAtMaturity ) const override;

		private:
			double strike_;
	};

	// European put payoff: max(K - S_T, 0).
	class PutPayoff : public Payoff {

		public:
			explicit PutPayoff( double strike );
			double operator()( double spotAtMaturity ) const override;

		private:
			double strike_;
	};
} // namespace pricing
