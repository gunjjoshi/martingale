#pragma once

#include "Payoff.hpp"

namespace pricing {

	class Option {

		public:
			Option ( const Payoff& payoff, double maturity );

			const Payoff& getPayoff() const;
			double getMaturity() const;

		private:
			const Payoff& payoff_;
			double maturity_;
	};
} // namespace pricing
