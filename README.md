# Alhambra

# Dependencies and Build
The project requires Qt. It was built using Qt 5.15. It uses CMake to build the project. CMake 3.16.4 was used. A C++ compiler supporting C++ 2020 is needed. Visual Studio 2019 was used.

A script to generate a Visual-Studio solution is provided. In order for CMake to find Qt, the environment variable QT5_DIR must be defined and must point to the Qt 5.12 directory. For example:

    QT5_DIR=C:\Outils\Qt\5.15.0\msvc2019_64

Alternatively, to invoke cmake directly, it needs to be told where to find Qt. It needs the environment variable CMAKE_PREFIX_PATH set to the location of Qt. For example:

    CMAKE_PREFIX_PATH=%QT5_DIR%\msvc2019_64

The code was written and tested with Visual Studio 2019, community edition.
