#include <dak/tiling/star.h>

#include <dak/tiling/rosette.h>
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

      std::shared_ptr<figure_t> star_t::clone() const
      {
         return std::make_shared<star_t>(*this);
      }

      bool star_t::operator==(const figure_t& other) const
      {
         const auto other_star = dynamic_cast<const star_t *>(&other);
         if (!other_star)
            return false;

         return n == other_star->n
             && s == other_star->s
             && d == other_star->d;
      }

      bool star_t::is_similar(const figure_t& other) const
      {
         const auto other_star = dynamic_cast<const star_t *>(&other);
         if (!other_star)
            return false;

         return n == other_star->n;
      }

      void star_t::make_similar(const figure_t& other)
      {
         if (const auto other_rosette = dynamic_cast<const rosette_t *>(&other))
         {
            s = other_rosette->s;
         }
         else if (const auto other_star = dynamic_cast<const star_t *>(&other))
         {
            d = other_star->d;
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
            d = other_irregular->d;
            s = other_irregular->s;
         }
      }

      std::wstring star_t::describe() const
      {
         std::wstringstream ss;
         ss << L::t(L"Star") << L" " << n;
         return ss.str();
      }

      bool star_t::is_cache_valid() const
      {
         return radial_figure_t::is_cache_valid()
            && cached_d_last_build_unit == d
            && cached_s_last_build_unit == s;
      }

      void star_t::update_cached_values() const
      {
         radial_figure_t::update_cached_values();
         cached_d_last_build_unit = d;
         cached_s_last_build_unit = s;
      }

      edges_map_t star_t::build_unit() const
      {
         const double clamp_d = std::max(1.0, std::min(d, 0.5 * n - 0.01));
         const double did = std::floor(clamp_d);

         double dfrac = clamp_d - did;
         int di = static_cast<int>(did);
         bool is_int = false;

         const int clamp_s = std::min(s, di);
         const int outer_s = std::min(s, di - 1);

         if (dfrac < TOLERANCE)
         {
            dfrac = 0.0;
            is_int = true;
         }
         else if ((1.0 - dfrac) < TOLERANCE)
         {
            dfrac = 0.0;
            di = di + 1;
            is_int = true;
         }

         edges_map_t from ;

         std::vector<point_t> points;
         points.reserve(clamp_s + 1);

         point_t a = point_t::unit_x();
         point_t b = geometry::get_arc(clamp_d / n);

         for (int i = 1; i <= outer_s; ++i)
         {
            const point_t ar = geometry::get_arc(i * 1. / n);
            const point_t br = geometry::get_arc((i - clamp_d) / n);

            point_t mid = intersect(a, b, ar, br);

            points.emplace_back(mid);
         }

         point_t top_prev = a;
         point_t bot_prev = a;

         for (int i = 0; i < points.size(); ++i)
         {
            const point_t top = points[i];
            const point_t bot = point_t(top.x, -top.y);

            from.insert(edge_t(top_prev, top));
            from.insert(edge_t(bot_prev, bot));

            top_prev = top;
            bot_prev = bot;
         }

         const transform_t trf = transform_t::rotate(2 * PI / n);
         if (clamp_s == di)
         {
            const point_t midr = top_prev.apply(trf);

            if (is_int)
            {
               from.insert(edge_t(top_prev, midr));
            }
            else
            {
               const point_t ar = geometry::get_arc(did / n);
               const point_t br = geometry::get_arc(-dfrac / n);

               const point_t c = geometry::get_arc(d / n);

               point_t cent = intersect(ar, br, a, c);
               from.insert(edge_t(top_prev, cent));
               from.insert(edge_t(cent, midr));
            }
         }

         return from;
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
