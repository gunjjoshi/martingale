#include "PathPayoff.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace pricing {

	ArithmeticAsianCallPayoff::ArithmeticAsianCallPayoff( double strike ) : strike_( strike ) {}

	double ArithmeticAsianCallPayoff::operator()( const std::vector<double>& path ) const {
		// Arithmetic average: (1/M) Σ S_{t_i}
		const double sum = std::accumulate( path.begin(), path.end(), 0.0 );
		const double average = sum / static_cast<double>( path.size() );
		return std::max( average - strike_, 0.0 );
	}

	ArithmeticAsianPutPayoff::ArithmeticAsianPutPayoff( double strike ) : strike_( strike ) {}

	double ArithmeticAsianPutPayoff::operator()( const std::vector<double>& path ) const {
		const double sum = std::accumulate( path.begin(), path.end(), 0.0 );
		const double average = sum / static_cast<double>( path.size() );
		return std::max( strike_ - average, 0.0 );
	}

	GeometricAsianCallPayoff::GeometricAsianCallPayoff( double strike ) : strike_( strike ) {}

	double GeometricAsianCallPayoff::operator()( const std::vector<double>& path ) const {
		// Compute the geometric average via log-sum to avoid floating-point overflow
		// when prices are large or the path is long:
		//   G = exp( (1/M) Σ ln(S_{t_i}) )
		// This is numerically identical to (Π S_i)^{1/M} but stays in a safe range.
		double logSum = 0.0;
		for ( const double s : path ) logSum += std::log( s );
		const double geoAverage = std::exp( logSum / static_cast<double>( path.size() ) );
		return std::max( geoAverage - strike_, 0.0 );
	}

	GeometricAsianPutPayoff::GeometricAsianPutPayoff( double strike ) : strike_( strike ) {}

	double GeometricAsianPutPayoff::operator()( const std::vector<double>& path ) const {
		// Compute the geometric average via log-sum to avoid floating-point overflow
		// when prices are large or the path is long:
		//   G = exp( (1/M) Σ ln(S_{t_i}) )
		// This is numerically identical to (Π S_i)^{1/M} but stays in a safe range.
		double logSum = 0.0;
		for ( const double s : path ) logSum += std::log( s );
		const double geoAverage = std::exp( logSum / static_cast<double>( path.size() ) );
		return std::max( strike_ - geoAverage, 0.0 );
	}

	DownAndOutCallPayoff::DownAndOutCallPayoff( double strike, double barrier )
		: strike_( strike ), barrier_( barrier ) {}

	double DownAndOutCallPayoff::operator()( const std::vector<double>& path ) const {
		// Walk the entire path. If the price ever touches or breaches the lower
		// barrier at any monitoring date, the option is knocked out immediately
		// and pays nothing regardless of where S_T ends up.
		for ( const double s : path ) {
			if ( s <= barrier_ ) return 0.0;   // knocked out
		}
		// Barrier was never breached: apply the vanilla call payoff at S_T.
		return std::max( path.back() - strike_, 0.0 );
	}

	UpAndOutCallPayoff::UpAndOutCallPayoff( double strike, double barrier )
		: strike_( strike ), barrier_( barrier ) {}

	double UpAndOutCallPayoff::operator()( const std::vector<double>& path ) const {
		// Walk the entire path. If the price ever touches or breaches the upper
		// barrier at any monitoring date, the option is knocked out immediately
		// and pays nothing regardless of where S_T ends up.
		for ( const double s : path ) {
			if ( s >= barrier_ ) return 0.0;   // knocked out
		}
		// Barrier was never breached: apply the vanilla call payoff at S_T.
		return std::max( path.back() - strike_, 0.0 );
	}

	// TODO: Add more types such as Down-and-out put, Up-and-out put, Down-and-in/Up-and-in call/put

} // namespace pricing
