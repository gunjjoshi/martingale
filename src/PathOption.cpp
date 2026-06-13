#include "PathOption.hpp"

namespace pricing {

	PathOption::PathOption( const PathPayoff& payoff, double maturity, std::uint64_t steps )
		: payoff_( payoff ), maturity_( maturity ), steps_( steps ) {}

	const PathPayoff& PathOption::getPayoff()   const { return payoff_;   }
	double            PathOption::getMaturity() const { return maturity_; }
	std::uint64_t     PathOption::getSteps()    const { return steps_;    }

} // namespace pricing
