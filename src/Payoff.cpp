#include "Payoff.hpp"
#include <algorithm>

namespace pricing {

	CallPayoff::CallPayoff( double strike ) : strike_( strike ) {}

	// In-the-money when S_T > K; otherwise the call expires worthless.
	double CallPayoff::operator()( double spotAtMaturity ) const {
		return std::max( spotAtMaturity - strike_, 0.0 );
	}

	PutPayoff::PutPayoff( double strike ) : strike_( strike ) {}

	// In-the-money when S_T < K; otherwise the put expires worthless.
	double PutPayoff::operator()( double spotAtMaturity ) const {
		return std::max( strike_ - spotAtMaturity, 0.0 );
	}
} // namespace pricing
