@echo off
setlocal
cd %~dp0
mkdir build
cd build
rem set QT5_DIR to where the Qt5 visual-studio 2019 64-bit files are installed,
rem for example C:\Outils\Qt\5.15.1\msvc2019_64
set QT5_DIR=C:\Outils\Qt-5.15\5.15.0\msvc2019_64
set CMAKE_PREFIX_PATH=%QT5_DIR%
git submodule update
cmake .. -G "Visual Studio 16 2019" -DCMAKE_GENERATOR_TOOLSET=v142

