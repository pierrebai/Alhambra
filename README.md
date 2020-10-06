# Introduction

Over a thousand years ago, artisans in the Islamic world began to
develop a system for constructing intricate geometric art based on radially
symmetric starlike figures. As the centuries progressed, they raised this
practice into a high art form, adorning architectural surfaces with colourful
symmetric patterns of limitless variety. The genre's masterpiece is surely the
Alhambra palace in Granada, Spain.  The era of building great bejewelled
palaces is behind us, and with its passing went the craftsmen who designed
these beautiful motifs. The techniques were closely guarded secrets that have
not been handed down to the present day. Thus, we are forced to re-engineer the
original design techniques from what clues survive.  Many different systems
have been hypothesized in modern times. What's weird is that most of them work,
even though they're all so different. In truth, we can't know for sure how the
Islamic artisans figured out these designs. But we can invent systems to create
designs similar to theirs. And revel in the exploration.


# Alhambra

Alhambra is a program that implements two such design technique for Islamic
star patterns. The first technique is based largely on the work of Hankin [1]
in the early part of the twentieth century and on a more recent paper by A.J.
Lee [2]. The technique was further refined by Craig S Kaplan [3]. In a
nutshell, we start with a tiling of plane made up at least in part of regular
polygons.  The polygons are filled with radially symmetric motifs like those
found in the Islamic tradition. The tiles forming the gaps between the regular
polygons are then filled in by finding natural extensions of the lines meeting
their boundaries. The result is a network of lines that has nice
graph-theoretic properties. The graph structure enables it to be coloured in
various ways, or even rendered as a weave, or interlacing, as were many of the
original designs.


The second technique is based on work by Peter J. Lu and Paul J. Steinhardt of
Harvard University [5]. It centers on the realization that many classical
Islamic tilings are based on a small set of tiles with equilateral sides, now
dubbed Girih tiles. When each tile is decorated with a particular set of lines,
they can be assembled to produce many of the intricate designs found in
mediaval Islamic architecture.


Alhambra has a library of built-in tilings that can be used to construct many
famous Islamic designs. Even better, the construction of these designs is
parameterized in certain ways, so you can use Alhambra as a vehicle for
exploration of the vast space of Islamic designs.


The research that went into this program appeared in print in the proceedings of
the third annual Bridges conference at Southwestern College in Kansas. You can
read the paper online [3] as part of the Bridges issue of the journal Visual
Mathematics. You can also view a PDF of the paper [4] as it appeared in the
Bridges proceedings.


![Alhambra User Interface](https://github.com/pierrebai/Alhambra/blob/master/App.png "Alhambra User Interface")


# Authors

- Original author: Craig S. Kaplan
- Additional work by: Pierre Baillargeon


# Licensing

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>. 


# More information

1. E.H. Hankin, Memoirs of the Archaeological Societry of India, volume 15. Government of India, 1925.
2. A.J. Lee. Islamic Star Patterns. Muqarnas, 4:182-197, 1995.
3. http://vismath4.tripod.com/kaplan/index.html
4. http://www.cgl.uwaterloo.ca/~csk/washington/tile/papers/kaplan_bridges2000.pdf
5. http://www.peterlu.org//content/decagonal-and-quasicrystalline-tilings-medieval-islamic-architecture


# Dependencies and Build
The project requires Qt. It was built using Qt 5.15. It uses CMake to build the project.
CMake 3.16.4 was used. A C++ compiler supporting C++ 2020 is needed. Visual Studio 2019 was used.

A script to generate a Visual-Studio solution is provided. In order for CMake to find Qt,
the environment variable QT5_DIR must be defined and must point to the Qt 5.12 directory.
For example:

    QT5_DIR=C:\Outils\Qt\5.15.0\msvc2019_64

Alternatively, to invoke cmake directly, it needs to be told where to find Qt.
It needs the environment variable CMAKE_PREFIX_PATH set to the location of Qt.
For example:

    CMAKE_PREFIX_PATH=%QT5_DIR%\msvc2019_64

The code was written and tested with Visual Studio 2019, community edition.
