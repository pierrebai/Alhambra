#pragma once

#ifndef DAK_GEOMETRY_CONSTANTS_H
#define DAK_GEOMETRY_CONSTANTS_H

namespace dak
{
   namespace geometry
   {
      constexpr double PI = 3.1415926535897932384626433832795028842;

      // Tolerance used when comparing point locations.
      // Thus it affects points, edges, polygons, etc.
      constexpr double TOLERANCE = 1e-7;

      // Square of the above. Used for squred distances.
      constexpr double TOLERANCE_2 = TOLERANCE * TOLERANCE;
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 

