#pragma once

#ifndef DAK_UTILITY_CONSTANTS_H
#define DAK_UTILITY_CONSTANTS_H

namespace dak
{
   namespace utility
   {
      // Tolerance used when comparing double values and points locations.
      // Thus it affects points, edges, polygons, etc.
      constexpr double TOLERANCE = 1e-7;

      // Square of the above. Used for squared distances.
      constexpr double TOLERANCE_2 = TOLERANCE * TOLERANCE;
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 

