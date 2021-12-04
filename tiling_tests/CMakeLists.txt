
add_library(tiling_tests SHARED
   src/tiling_io_tests.cpp
   src/tiling_tests.cpp
)

target_link_libraries(tiling_tests PUBLIC
   tiling
   utility
   geometry
)

target_compile_features(tiling_tests PUBLIC
   cxx_std_20
)

target_include_directories(tiling_tests PUBLIC
   "${PROJECT_SOURCE_DIR}/tiling/include"
   "${PROJECT_SOURCE_DIR}/dak/utility/include"
   "${PROJECT_SOURCE_DIR}/dak/geometry/include"
)

