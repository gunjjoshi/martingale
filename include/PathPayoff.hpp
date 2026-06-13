#pragma once

#include <vector>

namespace pricing {

	// Path-dependent payoff: maps an entire simulated price trajectory
	// to the option's payoff. Unlike Payoff (which acts on a single terminal
	// price S_T), a PathPayoff can inspect every price along the path, enabling
	// Asian averages, barrier knock-outs, lookbacks, and anything else that cares
	// about the route the asset took, not just where it ended up.
	class PathPayoff {

		public:
			virtual ~PathPayoff() = default;

			// path[i] is the simulated asset price at time step i+1.
			// path[0] is the price at the FIRST monitoring time (not S₀).
			// path.back() is always the terminal price S_T.
			virtual double operator()( const std::vector<double>& path ) const = 0;
	};

	// Asian options replace the terminal price in the payoff with an average of
	// prices observed at M monitoring dates. Averaging smooths out price spikes
	// near expiry, making Asian options cheaper and less manipulable than vanillas.
	// Arithmetic Asian call: max(A - K, 0), where A = (1/M) Σ S_{t_i}
	class ArithmeticAsianCallPayoff : public PathPayoff {

		public:
			explicit ArithmeticAsianCallPayoff( double strike );
			double operator()( const std::vector<double>& path ) const override;

		private:
			double strike_;
	};

	// Arithmetic Asian put: max(K - A, 0)
	class ArithmeticAsianPutPayoff : public PathPayoff {

		public:
			explicit ArithmeticAsianPutPayoff( double strike );
			double operator()( const std::vector<double>& path ) const override;

		private:
			double strike_;
	};

	// Geometric Asian call: max(G - K, 0), where G = (Π S_{t_i})^{1/M}
	// The geometric average of lognormals is itself lognormal, so this payoff
	// has an exact closed-form price under GBM.
	class GeometricAsianCallPayoff : public PathPayoff {

		public:
			explicit GeometricAsianCallPayoff( double strike );
			double operator()( const std::vector<double>& path ) const override;

		private:
			double strike_;
	};

	// Geometric Asian put: max(K - G, 0),
	// The geometric average of lognormals is itself lognormal, so this payoff
	// has an exact closed-form price under GBM.
	class GeometricAsianPutPayoff : public PathPayoff {

		public:
			explicit GeometricAsianPutPayoff( double strike );
			double operator()( const std::vector<double>& path ) const override;

		private:
			double strike_;
	};

	// Barrier options are the simplest path-dependent instruments: a vanilla
	// payoff with a conditional: the option either activates or extinguishes
	// if the asset crosses a pre-agreed barrier level during its life.
	//
	// Knock-out barriers ("out" options) extinguish the option upon breach.
	// Knock-in barriers ("in" options) only come alive upon breach.

	// Down-and-out call: pays max(S_T - K, 0) only if S_t > B for all t in [0,T].
	class DownAndOutCallPayoff : public PathPayoff {

		public:
			DownAndOutCallPayoff( double strike, double barrier );
			double operator()( const std::vector<double>& path ) const override;

		private:
			double strike_;
			double barrier_;
	};

	// Up-and-out call: pays max(S_T - K, 0) only if S_t < B for all t in [0,T].
	class UpAndOutCallPayoff : public PathPayoff {

		public:
			UpAndOutCallPayoff( double strike, double barrier );
			double operator()( const std::vector<double>& path ) const override;

		private:
			double strike_;
			double barrier_;
	};

	// TODO: Add more types such as Down-and-out put, Up-and-out put, Down-and-in/Up-and-in call/put

} // namespace pricing
