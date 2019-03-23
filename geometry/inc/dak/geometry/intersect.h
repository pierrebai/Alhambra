#pragma once

#ifndef DAK_GEOMETRY_FACES_H
#define DAK_GEOMETRY_FACES_H

#include <dak/geometry/point.h>

namespace dak
{
   namespace geometry
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // Some routines for testing line segment intersections.

      namespace intersect
      {
         // Return a point (s,t), where s is the fraction from p1 to
         // q1 where an intersection occurs.  t is defined similarly for p2 and q2.
         // If there's no intersection, return an invalid point.
         point get_intersection_params(const point& p1, const point& q1, const point& p2, const point& q2);

         // Coerce the point to be invalid if not on both unit segments.
         const point& stay_on_units(const point& ip);

         // Coerce the point to be invalid if too far off both unit segments.
         static constexpr double near_limit = 0.2;
         const point& stay_near_units(const point& ip);

         // Get the position of the intersection by interpolating.
         // Returns invalid if parallel or if it ends up outside of the segments.
         point intersect(const point& p1, const point& q1, const point& p2, const point& q2);

         // Get the position of the intersection by interpolating.
         // Returns invalid if parallel or if the point is too far off.
         point intersect_near(const point& p1, const point& q1, const point& p2, const point& q2);

         // Get the position of the intersection by interpolating, even outside the segments.
         // Returns invalid if parallel.
         point intersect_anywhere(const point& p1, const point& q1, const point& p2, const point& q2);

         // Don't return the intersection if it is at the enpoints of both segments.
         point intersect_within(const point& p1, const point& q1, const point& p2, const point& q2);
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
