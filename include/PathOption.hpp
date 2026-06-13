#pragma once

#include <cstdint>
#include "PathPayoff.hpp"

namespace pricing {

	// A path-dependent option contract: a PathPayoff observed at M equally-spaced
	// monitoring dates over [0, T].
	//
	// M controls the discretisation granularity.
	class PathOption {

		public:
			PathOption( const PathPayoff& payoff, double maturity, std::uint64_t steps );

			const PathPayoff& getPayoff()  const;
			double            getMaturity() const;
			std::uint64_t     getSteps()    const;

		private:
			const PathPayoff& payoff_;
			double            maturity_;
			std::uint64_t     steps_;
	};

} // namespace pricing
