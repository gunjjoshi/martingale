#pragma once

#include "generator.hpp"
#include <random>

namespace martingale::core::random {

/**
 * @brief Random number generator using std::random
 * 
 * Uses Mersenne Twister 19937 for uniform, Box-Muller for normal.
 */
class StdGenerator : public Generator {
public:
    explicit StdGenerator(uint64_t seed = std::random_device{}());
    
    double uniform() override;
    double normal() override;
    void seed(uint64_t s) override;

private:
    std::mt19937_64 gen_;
    std::uniform_real_distribution<double> uniform_dist_;
    std::normal_distribution<double> normal_dist_;
};

} // namespace martingale::core::random