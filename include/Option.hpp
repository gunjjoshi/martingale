#pragma once

namespace pricing {

	class Option {

		public:
			Option ( double strike, double maturity );

			double getStrike() const;
			double getMaturity() const;

		private:
			double strike_;
			double maturity_;
	};
} // namespace pricing
