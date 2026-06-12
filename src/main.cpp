#include <iomanip>
#include <iostream>
#include <MonteCarloPricer.hpp>
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

	const std::uint64_t simulations = 100000;

	Option option( strike, maturity );
	const double price = MonteCarloPricer::priceEuropeanCall( spot, rate, vol, option, simulations );

	std::cout << "European Call Option\n\n" << "Spot Price : " << spot << "\n" << "Strike     : " << strike << "\n" << "Volatility : " << vol * 100 << "%\n" << "Rate       : " << rate * 100 << "%\n" << "Maturity   : " << maturity << " year\n\n" << std::fixed << std::setprecision(2) << "Monte Carlo Price: " << price << std::endl;

	return 0;

}
