#pragma once

#include <cstdint>
#include <memory>

namespace martingale::core::random {

/**
 * @brief Abstract random number generator interface
 * 
 * Provides a uniform API for different RNG backends (std::random, MKL, etc.)
 */
class Generator {
public:
    virtual ~Generator() = default;
    
    /// Generate uniform random number in [0, 1)
    virtual double uniform() = 0;
    
    /// Generate standard normal random number N(0,1)
    virtual double normal() = 0;
    
    /// Set random seed for reproducibility
    virtual void seed(uint64_t s) = 0;
    
    /// Generate vector of uniform random numbers
    virtual void uniform(double* out, size_t n) {
        for (size_t i = 0; i < n; ++i) {
            out[i] = uniform();
        }
    }
    
    /// Generate vector of normal random numbers
    virtual void normal(double* out, size_t n) {
        for (size_t i = 0; i < n; ++i) {
            out[i] = normal();
        }
    }
};

/**
 * @brief Factory function to create default RNG
 * 
 * Returns std::random-based generator by default.
 * Can be swapped for MKL if available.
 */
std::unique_ptr<Generator> make_generator(uint64_t seed = 0);

} // namespace martingale::core::random