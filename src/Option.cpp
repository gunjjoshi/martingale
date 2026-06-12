#include "Option.hpp"

namespace pricing {

	Option::Option( const Payoff& payoff, double maturity ) : payoff_( payoff ), maturity_( maturity ) {}

	const Payoff& Option::getPayoff() const {
		return payoff_;
	}

	double Option::getMaturity() const {
		return maturity_;
	}
} // namespace pricing
