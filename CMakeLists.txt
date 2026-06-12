cmake_minimum_required(VERSION 3.16)

# The 'LANGUAGES CXX' clause tells CMake not to look for a C compiler, which speeds up the configure step on machines where cc is absent.
project(OptionPricer VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # If the compiler does not support C++20, fail loudly at configure time rather than silently falling back to an older standard and producing confusing compile errors later.

# Compile the engine into a static library called 'pricing_core'.
add_library( pricing_core src/Option.cpp src/Payoff.cpp src/MonteCarloPricer.cpp )

# Expose the 'include/' directory to anything that links pricing_core.
# PUBLIC means: this library uses it, AND so does any target that links it.
target_include_directories(pricing_core PUBLIC include)

add_executable(option_pricer src/main.cpp)
target_link_libraries(option_pricer PRIVATE pricing_core)

enable_testing()
add_executable(test_pricer tests/test_pricer.cpp)
target_link_libraries(test_pricer PRIVATE pricing_core)
add_test(NAME pricer_validation COMMAND test_pricer)
