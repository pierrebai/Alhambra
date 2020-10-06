# Qt 5 stuff

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)
find_package(Qt5 COMPONENTS WinExtras REQUIRED)

add_library(tiling_ui_qt
   include/dak/tiling_ui_qt/ask.h                  src/ask.cpp
   include/dak/tiling_ui_qt/drawing.h              src/drawing.cpp
   include/dak/tiling_ui_qt/tiling_canvas.h        src/tiling_canvas.cpp
   include/dak/tiling_ui_qt/mosaic_canvas.h        src/mosaic_canvas.cpp
   include/dak/tiling_ui_qt/figure_editor.h        src/figure_editor.cpp
   include/dak/tiling_ui_qt/figure_selector.h      src/figure_selector.cpp
   include/dak/tiling_ui_qt/layers_selector.h      src/layers_selector.cpp
   include/dak/tiling_ui_qt/main_window.h          src/main_window.cpp
   include/dak/tiling_ui_qt/styles_editor.h        src/styles_editor.cpp
   include/dak/tiling_ui_qt/tiling_editor.h        src/tiling_editor.cpp
   include/dak/tiling_ui_qt/tiling_selector.h      src/tiling_selector.cpp
   include/dak/tiling_ui_qt/tiling_window.h        src/tiling_window.cpp
)

target_include_directories(tiling_ui_qt PUBLIC
   include
   "${PROJECT_SOURCE_DIR}/tiling/include"
   "${PROJECT_SOURCE_DIR}/tiling_style/include"
   "${PROJECT_SOURCE_DIR}/dak_utility/utility/include"
   "${PROJECT_SOURCE_DIR}/dak_utility/geometry/include"
   "${PROJECT_SOURCE_DIR}/dak_utility/ui/include"
   "${PROJECT_SOURCE_DIR}/dak_utility/ui_qt/include"
   "${PROJECT_SOURCE_DIR}/QtAdditions/include"
)

target_include_directories(tiling_ui_qt PUBLIC
   "${Qt5Core_INCLUDE_DIRS}"
   "${Qt5Widgets_INCLUDE_DIRS}"
   "${Qt5Gui_INCLUDE_DIRS}"
   "${Qt5WinExtras_INCLUDE_DIRS}"
)

target_compile_features(tiling_ui_qt PUBLIC
   cxx_std_20
)

