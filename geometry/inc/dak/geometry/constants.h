#pragma once

#ifndef DAK_GEOMETRY_CONSTANTS_H
#define DAK_GEOMETRY_CONSTANTS_H

#include <dak/utility/constants.h>

namespace dak
{
   namespace geometry
   {
      constexpr double PI = 3.1415926535897932384626433832795028842;

      // Tolerance used when comparing double values and points locations.
      // Thus it affects points, edges, polygons, etc.
      constexpr double TOLERANCE = utility::TOLERANCE;

      // Square of the above. Used for squared distances.
      constexpr double TOLERANCE_2 = utility::TOLERANCE_2;
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 

