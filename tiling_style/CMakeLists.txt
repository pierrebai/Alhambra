
add_library(tiling_style
   include/dak/tiling_style/colored.h                 src/colored.cpp
   include/dak/tiling_style/emboss.h                  src/emboss.cpp
   include/dak/tiling_style/filled.h                  src/filled.cpp
   include/dak/tiling_style/interlace.h               src/interlace.cpp
   include/dak/tiling_style/known_mosaics.h           src/known_mosaics.cpp
   include/dak/tiling_style/known_mosaics_generator.h src/known_mosaics_generator.cpp
   include/dak/tiling_style/outline.h                 src/outline.cpp
   include/dak/tiling_style/plain.h                   src/plain.cpp
   include/dak/tiling_style/sketch.h                  src/sketch.cpp
   include/dak/tiling_style/style.h                   src/style.cpp
   include/dak/tiling_style/mosaic_io.h               src/mosaic_io.cpp
   include/dak/tiling_style/styled_mosaic.h           src/styled_mosaic.cpp
   include/dak/tiling_style/thick.h                   src/thick.cpp
)

target_include_directories(tiling_style PUBLIC
   include
)

target_link_libraries(tiling_style
   tiling dak_utility dak_geometry dak_ui
)

target_compile_features(tiling_style PUBLIC
   cxx_std_20
)

