# Qt 5 stuff

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Core REQUIRED)
find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt5 COMPONENTS Gui REQUIRED)
find_package(Qt5 COMPONENTS WinExtras REQUIRED)
find_package(Qt5 COMPONENTS Svg REQUIRED)

add_executable(Alhambra WIN32
   src/Alhambra.cpp

   resources/resource.rc                resources/resource.h

   "${PROJECT_SOURCE_DIR}/LICENSE"
   "${PROJECT_SOURCE_DIR}/README.md"
   "${PROJECT_SOURCE_DIR}/TODO.txt"
)

set_target_properties(Alhambra PROPERTIES OUTPUT_NAME "Alhambra")

target_link_libraries(Alhambra PUBLIC
   tiling
   tiling_style
   tiling_ui_qt
   utility
   geometry
   ui
   ui_qt
   QtAdditions
   Qt5::Widgets Qt5::Gui Qt5::Core Qt5::WinExtras Qt5::Svg
)

target_compile_features(Alhambra PUBLIC
   cxx_std_20
)

target_include_directories(Alhambra PUBLIC
   include
   resources
   "${PROJECT_SOURCE_DIR}/tiling/include"
   "${PROJECT_SOURCE_DIR}/tiling_style/include"
   "${PROJECT_SOURCE_DIR}/tiling_ui_qt/include"
   "${PROJECT_SOURCE_DIR}/dak_utility/utility/include"
   "${PROJECT_SOURCE_DIR}/dak_utility/geometry/include"
   "${PROJECT_SOURCE_DIR}/dak_utility/ui/include"
   "${PROJECT_SOURCE_DIR}/dak_utility/ui_qt/include"
   "${PROJECT_SOURCE_DIR}/QtAdditions/include"
)

add_custom_command(TARGET Alhambra POST_BUILD
   COMMAND "${Qt5_DIR}/../../../bin/windeployqt.exe" --no-webkit2 --no-angle --no-system-d3d-compiler --no-opengl-sw "$<TARGET_FILE:Alhambra>"
)

add_custom_command(TARGET Alhambra POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${PROJECT_SOURCE_DIR}/tiling/mosaics
                ${CMAKE_CURRENT_BINARY_DIR}/mosaics
)

add_custom_command(TARGET Alhambra POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${PROJECT_SOURCE_DIR}/tiling/tilings
                ${CMAKE_CURRENT_BINARY_DIR}/tilings
)

add_custom_command(TARGET Alhambra POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${PROJECT_SOURCE_DIR}/tiling/mosaics
                "$<TARGET_FILE_DIR:Alhambra>/mosaics"
)

add_custom_command(TARGET Alhambra POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${PROJECT_SOURCE_DIR}/tiling/tilings
                "$<TARGET_FILE_DIR:Alhambra>/tilings"
)

install(
   DIRECTORY
   "$<TARGET_FILE_DIR:Alhambra>/"
   DESTINATION .
   COMPONENT application
   PATTERN "*.pdb" EXCLUDE 
   PATTERN "*.ilk" EXCLUDE 
)

set(CPACK_GENERATOR NSIS)
set(CPACK_PACKAGE_NAME "Alhambra")
set(CPACK_PACKAGE_VENDOR "Pierre Baillargeon")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Alhambra - An application to create islamic tilings.")
set(CPACK_PACKAGE_VERSION "1.4.0")
set(CPACK_PACKAGE_VERSION_MAJOR "1")
set(CPACK_PACKAGE_VERSION_MINOR "4")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Alhambra")
set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE")

set(CPACK_NSIS_MUI_ICON "${PROJECT_SOURCE_DIR}/Alhambra/resources/icon.ico")
set(CPACK_NSIS_MUI_UNIICON "${PROJECT_SOURCE_DIR}/Alhambra/resources/icon.ico")
set(CPACK_NSIS_INSTALLED_ICON_NAME "Alhambra.exe")
set(CPACK_NSIS_DISPLAY_NAME "Alhambra")
set(CPACK_NSIS_CREATE_ICONS_EXTRA
    "CreateShortCut '$SMPROGRAMS\\\\$STARTMENU_FOLDER\\\\Alhambra.lnk' '$INSTDIR\\\\Alhambra.exe'"
)
set(CPACK_NSIS_DELETE_ICONS_EXTRA
    "Delete '$SMPROGRAMS\\\\$START_MENU\\\\Alhambra.lnk'"
)
INCLUDE(CPack)

