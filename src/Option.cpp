#include "Option.hpp"

namespace pricing {

	Option::Option( double strike, double maturity ) : strike_( strike ), maturity_( maturity ) {}

	double Option::getStrike() const {
		return strike_;
	}

	double Option::getMaturity() const {
		return maturity_;
	}
} // namespace pricing
