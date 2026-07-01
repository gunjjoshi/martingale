add_library(martingale-core-random
    src/std_random.cpp
)

target_include_directories(martingale-core-random
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

add_library(martingale::core::random ALIAS martingale-core-random)