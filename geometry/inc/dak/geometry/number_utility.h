#pragma once

#ifndef DAK_GEOMETRY_NUMBER_UTILITY_H
#define DAK_GEOMETRY_NUMBER_UTILITY_H

#include <dak/geometry/constants.h>

#include <cmath>

namespace dak
{
   namespace geometry
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // Floating-point functions.

      // Verify if the value is near zero for the given tolerance.
      inline bool near_zero(double a, double tolerance)
      {
         return std::abs(a) <= tolerance;
      }

      // Verify if the value is near zero for the default tolerance.
      inline bool near_zero(double a)
      {
         return near_zero(a, TOLERANCE);
      }

      // Verify if the two values are near for the given tolerance.
      inline bool near(double a, double b, double tolerance)
      {
         return near_zero(a - b, tolerance);
      }

      // Verify if the two values are near for the default tolerance.
      inline bool near(double a, double b)
      {
         return near(a, b, TOLERANCE);
      }

      // Verify if one value is less than another for the default tolerance.
      inline bool near_less(double a, double b)
      {
         return a < (b - TOLERANCE);
      }

      // Verify if a value is less than zero.
      inline bool near_negative(double a)
      {
         return near_less(a, 0);
      }

      // Verify if one value is more than another for the default tolerance.
      inline bool near_greater(double a, double b)
      {
         return a > (b + TOLERANCE);
      }

      // Verify if a value is more than zero.
      inline bool near_positive(double a)
      {
         return near_greater(a, 0);
      }

   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 

