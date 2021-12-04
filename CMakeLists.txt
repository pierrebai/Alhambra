cmake_minimum_required(VERSION 3.7.0)

# The Alhambra application and its tests.

project(Alhambra)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

add_subdirectory(QtAdditions)

add_subdirectory(dak/utility)
add_subdirectory(dak/geometry)
add_subdirectory(dak/ui)
add_subdirectory(dak/ui_qt)

add_subdirectory(tiling)
add_subdirectory(tiling_tests)
add_subdirectory(tiling_style)
add_subdirectory(tiling_ui_qt)

add_subdirectory(Alhambra)

