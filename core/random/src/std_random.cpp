#include "martingale/core/random/std_random.hpp"

namespace martingale::core::random {

StdGenerator::StdGenerator(uint64_t seed)
    : gen_(seed)
    , uniform_dist_(0.0, 1.0)
    , normal_dist_(0.0, 1.0)
{}

double StdGenerator::uniform() {
    return uniform_dist_(gen_);
}

double StdGenerator::normal() {
    return normal_dist_(gen_);
}

void StdGenerator::seed(uint64_t s) {
    gen_.seed(s);
}

// Factory implementation
std::unique_ptr<Generator> make_generator(uint64_t seed) {
    return std::make_unique<StdGenerator>(seed);
}

} // namespace martingale::core::random