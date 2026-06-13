#include <iomanip>
#include <iostream>
#include <MonteCarloPricer.hpp>
#include "Payoff.hpp"
#include "PathPayoff.hpp"
#include "PathOption.hpp"
using namespace pricing;

int main() {

	// S₀: current stock price.
	const double spot = 100.0;

	// K: the at-the-money case where spot == strike.
	const double strike = 100.0;

	// r: 5% annualised risk-free rate.
	const double rate = 0.05;

	// σ: 20% annualised volatility.
	const double vol = 0.20;

	// T: 1 year to expiry.
	const double maturity = 1.0;

	// B↓: down-and-out barrier
	const double barrierDn = 80.0;

	// B↑: up-and-out barrier
	const double barrierUp = 130.0;

	const std::uint64_t simulations = 100000;
	const std::uint64_t steps = 252; // Daily monitoring over 1 year

	// European call and put:

	CallPayoff call( strike );
	PutPayoff put( strike );

	Option callOption( call, maturity );
	Option putOption( put, maturity );

	const double callPrice = MonteCarloPricer::price( spot, rate, vol, callOption, simulations );
	const double putPrice = MonteCarloPricer::price( spot, rate, vol, putOption, simulations );

	std::cout << "European Options\n\n"
			<< "Spot Price : " << spot << "\n"
			<< "Strike     : " << strike << "\n"
			<< "Volatility : " << vol * 100 << "%\n"
			<< "Rate       : " << rate * 100 << "%\n"
			<< "Maturity   : " << maturity << " year\n\n"
			<< std::fixed << std::setprecision(2)
			<< "Monte Carlo Call Price: " << callPrice << "\n"
			<< "Monte Carlo Put  Price: " << putPrice << std::endl;


	// Arithmetic Asian options:

	ArithmeticAsianCallPayoff arithAsianCall( strike );
	ArithmeticAsianPutPayoff arithAsianPut( strike );

	PathOption arithAsianCallOpt( arithAsianCall, maturity, steps );
	PathOption arithAsianPutOpt( arithAsianPut, maturity, steps );

	const double arithCallPrice = MonteCarloPricer::pricePath( spot, rate, vol, arithAsianCallOpt, simulations );
	const double arithPutPrice = MonteCarloPricer::pricePath( spot, rate, vol, arithAsianPutOpt, simulations );

	std::cout << "Arithmetic Asian Options\n\n"
			<< "M : " << steps << "\n"
			<< "Spot Price : " << spot << "\n"
			<< "Strike     : " << strike << "\n"
			<< "Volatility : " << vol * 100 << "%\n"
			<< "Rate       : " << rate * 100 << "%\n"
			<< "Maturity   : " << maturity << " year\n\n"
			<< std::fixed << std::setprecision(2)
			<< "Monte Carlo Call Price: " << arithCallPrice << "\n"
			<< "Monte Carlo Put  Price: " << arithPutPrice << std::endl;


	// Geometric Asian options:

	GeometricAsianCallPayoff geoAsianCall( strike );
	GeometricAsianPutPayoff geoAsianPut( strike );

	PathOption geoAsianCallOpt( geoAsianCall, maturity, steps );
	PathOption geoAsianPutOpt( geoAsianPut, maturity, steps );

	const double geoCallPrice = MonteCarloPricer::pricePath( spot, rate, vol, geoAsianCallOpt, simulations );
	const double geoPutPrice = MonteCarloPricer::pricePath( spot, rate, vol, geoAsianPutOpt, simulations );

	std::cout << "Geometric Asian Options\n\n"
			<< "M : " << steps << "\n"
			<< "Spot Price : " << spot << "\n"
			<< "Strike     : " << strike << "\n"
			<< "Volatility : " << vol * 100 << "%\n"
			<< "Rate       : " << rate * 100 << "%\n"
			<< "Maturity   : " << maturity << " year\n\n"
			<< std::fixed << std::setprecision(2)
			<< "Monte Carlo Call Price: " << geoCallPrice << "\n"
			<< "Monte Carlo Put  Price: " << geoPutPrice << std::endl;


	// Barrier options:

	DownAndOutCallPayoff doc( strike, barrierDn );
	UpAndOutCallPayoff uoc( strike, barrierUp );

	PathOption docOpt( doc, maturity, steps );
	PathOption uocOpt( uoc, maturity, steps );

	const double docPrice = MonteCarloPricer::pricePath( spot, rate, vol, docOpt, simulations );
	const double uocPrice = MonteCarloPricer::pricePath( spot, rate, vol, uocOpt, simulations );

	std::cout << "Barrier Options\n\n"
			<< "M : " << steps << "\n"
			<< "Spot Price : " << spot << "\n"
			<< "Strike     : " << strike << "\n"
			<< "Volatility : " << vol * 100 << "%\n"
			<< "Rate       : " << rate * 100 << "%\n"
			<< "Maturity   : " << maturity << " year\n\n"
			<< std::fixed << std::setprecision(2)
			<< "  Down-and-Out Call ( B = " << barrierDn << " ) : " << docPrice << "\n"
			<< "  Up-and-Out   Call ( B = " << barrierUp << " ) : " << uocPrice << std::endl;

	return 0;
}
