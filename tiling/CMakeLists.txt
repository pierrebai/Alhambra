
add_library(tiling
   include/dak/tiling/explicit_figure.h      src/explicit_figure.cpp
   include/dak/tiling/extended_figure.h      src/extended_figure.cpp
   include/dak/tiling/figure.h               src/figure.cpp
   include/dak/tiling/infer.h                src/infer.cpp
   include/dak/tiling/infer_helpers.h
   include/dak/tiling/infer_mode.h
   include/dak/tiling/irregular_figure.h     src/irregular_figure.cpp
   include/dak/tiling/known_tilings.h        src/known_tilings.cpp
   include/dak/tiling/mosaic.h               src/mosaic.cpp
   include/dak/tiling/radial_figure.h        src/radial_figure.cpp
   include/dak/tiling/rosette.h              src/rosette.cpp
   include/dak/tiling/inflation_tiling.h     src/inflation_tiling.cpp
   include/dak/tiling/scale_figure.h         src/scale_figure.cpp
   include/dak/tiling/star.h                 src/star.cpp
   include/dak/tiling/tiling.h               src/tiling.cpp
   include/dak/tiling/tiling_io.h            src/tiling_io.cpp
   include/dak/tiling/tiling_selection.h     src/tiling_selection.cpp
   include/dak/tiling/translation_tiling.h   src/translation_tiling.cpp
)

target_include_directories(tiling PUBLIC
   include
   "${PROJECT_SOURCE_DIR}/dak/utility/include"
   "${PROJECT_SOURCE_DIR}/dak/geometry/include"
)

target_compile_features(tiling PUBLIC
   cxx_std_20
)

