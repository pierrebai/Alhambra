#pragma once

#ifndef DAK_GEOMETRY_UTILITY_H
#define DAK_GEOMETRY_UTILITY_H

#include <dak/utility/number.h>

#include <dak/geometry/constants.h>
#include <dak/geometry/point.h>

#include <vector>
#include <functional>

namespace dak
{
   namespace geometry
   {
      class transform;
      class rect;

      ////////////////////////////////////////////////////////////////////////////
      //
      // Point functions.

      // Verify if the point is near the origin for the given tolerance.
      bool near_zero(const point& a, double tolerance);

      // Verify if the point is near the origin for the default tolerance.
      bool near_zero(const point& a);

      // Verify if the two points are near for the given tolerance.
      bool near(const point& a, const point& b, double tolerance);

      // Verify if the two points are near for the default tolerance.
      bool near(const point& a, const point& b);

      // Verify if three points are co-linear.
      inline bool is_colinear(const point& p, const point& q, const point& a)
      {
         double px = p.x;
         double py = p.y;

         double qx = q.x;
         double qy = q.y;

         double x = a.x;
         double y = a.y;

         double left = (qx - px) * (y - py);
         double right = (qy - py) * (x - px);

         return utility::near(left, right);
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // Point calculation functions.

      // Calculate the center of all the given points.
      point center(const point* begin, const point* end);

      // Calculate the center of all the given points.
      point center(const std::vector<point>& pts);

      // Translate all the given points by the given amount.
      std::vector<point> translate(const std::vector<point>& pts, const point& move);

      // Apply the transform to all the points.
      std::vector<point> apply(const transform& t, const std::vector<point>& pts);

      // Get the point frac of the way around the unit circle.
      point get_arc(double frac);

      ////////////////////////////////////////////////////////////////////////////
      //
      // Fill a rectangular region with regularly spaced copies of a motif.
      // fill a rectangle regions with the integer linear combinations of two
      // translation vectors.
      //
      // To make the algorithm general, the output is provided through a 
      // callback that gets a sequence of calls, one for each translate.
      //
      // Calls the function repatedly with integer linear combinations to be applied to T1 and T2.
      size_t count_fill_replications(const rect& region, const point& T1, const point& T2);
      void fill(const rect& region, const point& T1, const point& T2, std::function<void(int, int)> c);
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 

