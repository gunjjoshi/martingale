# Fetch Eigen (header-only)
include(FetchContent)
FetchContent_Declare(
    Eigen3
    GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
    GIT_TAG 3.4.0
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(Eigen3)

# Header-only library
add_library(martingale-core-linalg INTERFACE)

target_include_directories(martingale-core-linalg
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_link_libraries(martingale-core-linalg
    INTERFACE Eigen3::Eigen
)

add_library(martingale::core::linalg ALIAS martingale-core-linalg)