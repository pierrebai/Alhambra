
add_library(tiling_tests SHARED
   src/tiling_io_tests.cpp
   src/tiling_tests.cpp
)

target_link_libraries(tiling_tests PUBLIC
   tiling
   dak_utility
   dak_geometry
)

target_compile_features(tiling_tests PUBLIC
   cxx_std_20
)

