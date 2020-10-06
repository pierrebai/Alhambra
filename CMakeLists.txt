cmake_minimum_required(VERSION 3.7.0)

# The Alhambra application and its tests.

project(Alhambra)

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

add_subdirectory(QtAdditions)

add_subdirectory(dak_utility/utility)
add_subdirectory(dak_utility/geometry)
add_subdirectory(dak_utility/ui)
add_subdirectory(dak_utility/ui_qt)

add_subdirectory(tiling)
add_subdirectory(tiling_tests)
add_subdirectory(tiling_style)
add_subdirectory(tiling_ui_qt)

add_subdirectory(Alhambra)

