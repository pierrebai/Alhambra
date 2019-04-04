#include <dak/geometry/utility.h>
#include <dak/geometry/transform.h>

#include <algorithm>

namespace dak
{
   namespace geometry
   {
      bool near_zero(const point& a, double tolerance)
      {
         return a.mag_2() <= tolerance;
      }

      bool near_zero(const point& a)
      {
         return near_zero(a, TOLERANCE_2);
      }

      bool near(const point& a, const point& b, double tolerance)
      {
         return a.distance_2(b) <= tolerance;
      }

      bool near(const point& a, const point& b)
      {
         return near(a, b, TOLERANCE_2);
      }

      point center(const point* begin, const point* end)
      {
         point cent = point(0.0, 0.0);
         const auto count = end - begin;
         while (begin != end)
            cent = cent + *begin++;
         return cent.scale(1.0 / (double) count);
      }

      point center(const std::vector<point>& pts)
      {
         if (pts.size() <= 0)
            return point();
         return center(&*pts.begin(), (&*pts.begin()) + pts.size());
      }

      std::vector<point> translate(const std::vector<point>& pts, const point& move)
      {
         std::vector<point> new_pts;
         new_pts.reserve(pts.size());
         for (const auto& pt : pts)
            new_pts.emplace_back(pt + move);
         return new_pts;
      }

      std::vector<point> apply(const transform& t, const std::vector<point>& pts)
      {
         std::vector<point> new_pts;
         new_pts.reserve(pts.size());
         for (const auto& pt : pts)
            new_pts.emplace_back(pt.apply(t));
         return new_pts;
      }

      point get_arc(double frac)
      {
         const double ang = frac * 2 * PI;
         return point(std::cos(ang), std::sin(ang));
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 

