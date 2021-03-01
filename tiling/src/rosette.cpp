#include <dak/tiling/rosette.h>

#include <dak/tiling/star.h>
#include <dak/tiling/extended_figure.h>
#include <dak/tiling/irregular_figure.h>

#include <dak/geometry/constants.h>
#include <dak/geometry/intersect.h>
#include <dak/geometry/transform.h>
#include <dak/geometry/utility.h>

#include <dak/utility/text.h>

#include <cmath>
#include <algorithm>
#include <sstream>

namespace dak
{
   namespace tiling
   {
      using geometry::transform_t;
      using geometry::point_t;
      using namespace geometry::intersect;
      using geometry::edge_t;
      using geometry::edges_map_t;
      using geometry::PI;
      using geometry::TOLERANCE;
      using utility::L;

      rosette_t::rosette_t(int n, double q, int s)
      : radial_figure_t(n), q(q), s(std::min(s, (n - 1) / 2))
      {
      }

      std::shared_ptr<figure_t> rosette_t::clone() const
      {
         return std::make_shared<rosette_t>(*this);
      }

      bool rosette_t::operator==(const figure_t& other) const
      {
         const auto other_rosette = dynamic_cast<const rosette_t *>(&other);
         if (!other_rosette)
            return false;

         return n == other_rosette->n
             && s == other_rosette->s
             && q == other_rosette->q;
      }

      bool rosette_t::is_similar(const figure_t& other) const
      {
         const auto other_rosette = dynamic_cast<const rosette_t *>(&other);
         if (!other_rosette)
            return false;

         return n == other_rosette->n;
      }

      void rosette_t::make_similar(const figure_t& other)
      {
         if (const auto other_rosette = dynamic_cast<const rosette_t *>(&other))
         {
            q = other_rosette->q;
            s = other_rosette->s;
         }
         else if (const auto other_star = dynamic_cast<const star_t *>(&other))
         {
            s = other_star->s;
         }
         else if (const auto other_extended = dynamic_cast<const extended_figure_t *>(&other))
         {
            if (other_extended->child)
            {
               make_similar(*other_extended->child);
            }
         }
         else if (const auto other_irregular = dynamic_cast<const irregular_figure_t *>(&other))
         {
            q = other_irregular->q;
            s = other_irregular->s;
         }
      }

      std::wstring rosette_t::describe() const
      {
         std::wstringstream ss;
         ss << L::t(L"Rosette") << L" " << n;
         return ss.str();
      }

      bool rosette_t::is_cache_valid() const
      {
         return radial_figure_t::is_cache_valid()
             && cached_q_last_build_unit == q
             && cached_s_last_build_unit == s;
      }

      void rosette_t::update_cached_values() const
      {
         radial_figure_t::update_cached_values();
         cached_q_last_build_unit = q;
         cached_s_last_build_unit = s;
      }

      edges_map_t rosette_t::build_unit() const
      {
         const double don = 1. / n;

         point_t tip = point_t(1.0, 0.0);         // The point to build from
         point_t rtip = geometry::get_arc(don);   // The next point over.

         double q_clamp = std::min(std::max(q, -0.99), 0.99);
         int s_clamp = std::min(s, (n - 1) / 2);

         // Consider an equilateral triangle formed by the origin,
         // up_outer and a vertical edge extending down from up_outer.
         // The center of the bottom edge of that triangle defines the
         // bisector of the angle leaving up_outer that we care about.
         const double r_outer = 1.0 / std::cos(PI * don);
         const point_t up_outer = geometry::get_arc(0.5 * don).scale(r_outer);
         const point_t bisector = (up_outer - point_t(0.0, r_outer)).scale(0.5);

         double th = (rtip - tip).angle();

         const point_t stable_isect = up_outer +  up_outer.normalize().scale(-up_outer.y);
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
         const point_t qtip = point_t(1.0 + std::cos(th), std::sin(th));

         const point_t key_point = intersect(tip, qtip, up_outer, bisector);

         const point_t key_end = key_point.convex_sum(stable_isect, 10.0);

         std::vector<point_t> points;
         points.reserve(s_clamp + 2);

         points.emplace_back(key_point);

         point_t key_r_p = point_t(key_point.x, -key_point.y);
         point_t key_r_e = point_t(key_end.x, -key_end.y);

         const auto trf = transform_t::rotate(2 * PI / n);
         for (int i = 1; i <= s_clamp; ++i)
         {
            key_r_p = key_r_p.apply(trf);
            key_r_e = key_r_e.apply(trf);

            const point_t mid = intersect(key_point, key_end, key_r_p, key_r_e);

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

         edges_map_t from;

         point_t top_prev = tip;
         point_t bot_prev = tip;

         for (int i = 0; i < points.size(); ++i)
         {
            const point_t& top = points[i];
            const point_t bot = point_t(top.x, -top.y);

            from.insert(edge_t(top_prev, top));
            from.insert(edge_t(bot_prev, bot));

            top_prev = top;
            bot_prev = bot;
         }

         return from;
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 

