#include <dak/tiling/rosette.h>

#include <dak/tiling/star.h>
#include <dak/tiling/extended_figure.h>
#include <dak/tiling/irregular_figure.h>

#include <dak/geometry/constants.h>
#include <dak/geometry/intersect.h>
#include <dak/geometry/transform.h>
#include <dak/geometry/utility.h>

#include <cmath>
#include <algorithm>
#include <sstream>

namespace dak
{
   namespace tiling
   {
      using geometry::transform;
      using geometry::point;
      using namespace geometry::intersect;
      using geometry::edge;
      using geometry::map;
      using geometry::PI;
      using geometry::TOLERANCE;
      using geometry::L;

      rosette::rosette(int n, double q, int s)
      : radial_figure(n), q(q), s(std::min(s, (n - 1) / 2))
      {
      }

      std::shared_ptr<figure> rosette::clone() const
      {
         return std::make_shared<rosette>(*this);
      }

      bool rosette::is_similar(const figure& other) const
      {
         const auto other_rosette = dynamic_cast<const rosette *>(&other);
         if (!other_rosette)
            return false;

         return n == other_rosette->n;
      }

      void rosette::make_similar(const figure& other)
      {
         if (const auto other_rosette = dynamic_cast<const rosette *>(&other))
         {
            q = other_rosette->q;
            s = other_rosette->s;
         }
         else if (const auto other_star = dynamic_cast<const star *>(&other))
         {
            s = other_star->s;
         }
         else if (const auto other_extended = dynamic_cast<const extended_figure *>(&other))
         {
            if (other_extended->child)
            {
               make_similar(*other_extended->child);
            }
         }
         else if (const auto other_irregular = dynamic_cast<const irregular_figure *>(&other))
         {
            q = other_irregular->q;
            s = other_irregular->s;
         }
      }

      std::wstring rosette::describe() const
      {
         std::wstringstream ss;
         ss << L::t(L"Rosette") << L" " << n;
         return ss.str();
      }

      bool rosette::is_cache_valid() const
      {
         return radial_figure::is_cache_valid()
             && cached_q_last_build_unit == q
             && cached_s_last_build_unit == s;
      }

      void rosette::update_cached_values() const
      {
         radial_figure::update_cached_values();
         cached_q_last_build_unit = q;
         cached_s_last_build_unit = s;
      }

      map rosette::build_unit() const
      {
         const double don = 1. / n;

         point tip = point(1.0, 0.0);           // The point to build from
         point rtip = geometry::get_arc(don);   // The next point over.

         double q_clamp = std::min(std::max(q, -0.99), 0.99);
         int s_clamp = std::min(s, (n - 1) / 2);

         // Consider an equilateral triangle formed by the origin,
         // up_outer and a vertical edge extending down from up_outer.
         // The center of the bottom edge of that triangle defines the
         // bisector of the angle leaving up_outer that we care about.
         const double r_outer = 1.0 / std::cos(PI * don);
         const point up_outer = geometry::get_arc(0.5 * don).scale(r_outer);
         const point bisector = (up_outer - point(0.0, r_outer)).scale(0.5);

         double th = (rtip - tip).angle();

         const point stable_isect = up_outer +  up_outer.normalize().scale(-up_outer.y);
         const double stable_angle = (stable_isect - tip).angle();

         if (q_clamp >= 0.0)
         {
            th = th * (1.0 - q_clamp) + (PI * 0.5) * q_clamp;
         }
         else
         {
            // th = th * (1.0 - (-q_clamp)) + std::PI * (-q_clamp);
            th = th * (1.0 + q_clamp) - stable_angle * q_clamp;
         }

         // Heh heh - you said q-tip - heh heh.
         const point qtip = point(1.0 + std::cos(th), std::sin(th));

         const point key_point = intersect(tip, qtip, up_outer, bisector);

         const point key_end = key_point.convex_sum(stable_isect, 10.0);

         std::vector<point> points;
         points.reserve(s_clamp + 2);

         points.emplace_back(key_point);

         point key_r_p = point(key_point.x, -key_point.y);
         point key_r_e = point(key_end.x, -key_end.y);

         const transform trf = transform::rotate(2 * PI / n);
         for (int i = 1; i <= s_clamp; ++i)
         {
            key_r_p = key_r_p.apply(trf);
            key_r_e = key_r_e.apply(trf);

            const point mid = intersect(key_point, key_end, key_r_p, key_r_e);

            // FIXME --
            // For some combinations of n, q and s (for example, 
            // n = 12, q = -0.8, s = 4), the intersection fails
            // because the line segments being checked end up 
            // parallel.  Rather than knowing mathematically when
            // that happens, I punt and check after the fact whether
            // the intersection test failed.

            if (!mid.is_invalid())
               points.emplace_back(mid);
         }

         map from;

         point top_prev = tip;
         point bot_prev = tip;

         for (int i = 0; i < points.size(); ++i)
         {
            const point& top = points[i];
            const point bot = point(top.x, -top.y);

            from.insert(edge(top_prev, top));
            from.insert(edge(bot_prev, bot));

            top_prev = top;
            bot_prev = bot;
         }

         return from;
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 

