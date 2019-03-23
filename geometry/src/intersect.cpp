#include <dak/geometry/intersect.h>
#include <dak/geometry/constants.h>
#include <dak/geometry/point.h>
#include <dak/geometry/utility.h>

namespace dak
{
   namespace geometry
   {
      point intersect::get_intersection_params(const point& p1, const point& q1, const point& p2, const point& q2)
      {
         const double p1x = p1.x;
         const double p1y = p1.y;

         const double q1x = q1.x;
         const double q1y = q1.y;

         const double p2x = p2.x;
         const double p2y = p2.y;

         const double q2x = q2.x;
         const double q2y = q2.y;

         const double d1x = q1x - p1x;
         const double d1y = q1y - p1y;
         const double d2x = q2x - p2x;
         const double d2y = q2y - p2y;

         const double det = (d1x * d2y) - (d1y * d2x);

         if (near_zero(det)) {
            // Parallel.  We won't worry about cases where endpoints touch
            // and we certainly won't worry about overlapping lines.
            // That leaves, um, nothing.  Done!
            return point::invalid;
         }

         // These two lines are adapted from O'Rourke's
         // _Computational Geometry in C_ segment-segment intersection code.
         const double is = -((p1x*d2y) + p2x * (p1y - q2y) + q2x * (p2y - p1y)) / det;
         const double it = ((p1x*(p2y - q1y)) + q1x * (p1y - p2y) + p2x * d1y) / det;

         return point(is, it);
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // Coerce the point to be invalid if not on both unit segments.

      const point& intersect::stay_on_units(const point& ip)
      {
         const double is = ip.x;
         if ((is < -TOLERANCE) || (is > (1.0 + TOLERANCE)))
            return point::invalid;

         double it = ip.y;
         if ((it < -TOLERANCE) || (it > (1.0 + TOLERANCE)))
            return point::invalid;

         return ip;
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // Coerce the point to be invalid if too far off both unit segments.

      const point& intersect::stay_near_units(const point& ip)
      {
         double is = ip.x;
         if (is < -near_limit || is > 1.0 + near_limit)
            return point::invalid;

         double it = ip.y;
         if (it < -near_limit || it > 1.0 + near_limit)
            return point::invalid;

         return ip;
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // Get the position of the intersection by interpolating.
      // Returns invalid if parallel or if it ends up outside of the segments.

      point intersect::intersect(const point& p1, const point& q1, const point& p2, const point& q2)
      {
         const point& ip = stay_on_units(get_intersection_params(p1, q1, p2, q2));
         if (ip.is_invalid())
            return ip;
         return p1.convex_sum(q1, ip.x);
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // Get the position of the intersection by interpolating.
      // Returns invalid if parallel or if the point is too far off.

      point intersect::intersect_near(const point& p1, const point& q1, const point& p2, const point& q2)
      {
         const point& ip = stay_near_units(get_intersection_params(p1, q1, p2, q2));
         if (ip.is_invalid())
            return ip;
         return p1.convex_sum(q1, ip.x);
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // Get the position of the intersection by interpolating, even outside the segments.
      // Returns invalid if parallel.

      point intersect::intersect_anywhere(const point& p1, const point& q1, const point& p2, const point& q2)
      {
         const point& ip = get_intersection_params(p1, q1, p2, q2);
         if (ip.is_invalid())
            return ip;
         return p1.convex_sum(q1, ip.x);
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // Don't return the intersection if it is at the enpoints of 
      // both segments.

      point intersect::intersect_within(const point& p1, const point& q1, const point& p2, const point& q2)
      {
         const point& ip = stay_on_units(get_intersection_params(p1, q1, p2, q2));
         if (ip.is_invalid())
            return ip;

         double s = ip.x;
         double t = ip.y;

         if (s <= TOLERANCE) {
            if ((t <= TOLERANCE) || ((1.0 - t) <= TOLERANCE)) {
               return point::invalid;
            }
         }
         else if ((1.0 - s) <= TOLERANCE) {
            if ((t <= TOLERANCE) || ((1.0 - t) <= TOLERANCE)) {
               return point::invalid;
            }
         }

         return p1.convex_sum(q1, s);
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
