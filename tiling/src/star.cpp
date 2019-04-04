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
      using geometry::transform;
      using geometry::point;
      using namespace geometry::intersect;
      using geometry::edge;
      using geometry::map;
      using geometry::PI;
      using geometry::TOLERANCE;
      using utility::L;

      std::shared_ptr<figure> star::clone() const
      {
         return std::make_shared<star>(*this);
      }

      bool star::operator==(const figure& other) const
      {
         const auto other_star = dynamic_cast<const star *>(&other);
         if (!other_star)
            return false;

         return n == other_star->n
             && s == other_star->s
             && d == other_star->d;
      }

      bool star::is_similar(const figure& other) const
      {
         const auto other_star = dynamic_cast<const star *>(&other);
         if (!other_star)
            return false;

         return n == other_star->n;
      }

      void star::make_similar(const figure& other)
      {
         if (const auto other_rosette = dynamic_cast<const rosette *>(&other))
         {
            s = other_rosette->s;
         }
         else if (const auto other_star = dynamic_cast<const star *>(&other))
         {
            d = other_star->d;
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
            d = other_irregular->d;
            s = other_irregular->s;
         }
      }

      std::wstring star::describe() const
      {
         std::wstringstream ss;
         ss << L::t(L"Star") << L" " << n;
         return ss.str();
      }

      bool star::is_cache_valid() const
      {
         return radial_figure::is_cache_valid()
            && cached_d_last_build_unit == d
            && cached_s_last_build_unit == s;
      }

      void star::update_cached_values() const
      {
         radial_figure::update_cached_values();
         cached_d_last_build_unit = d;
         cached_s_last_build_unit = s;
      }

      map star::build_unit() const
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

         map from ;

         std::vector<point> points;
         points.reserve(clamp_s + 1);

         point a = point::unit_x();
         point b = geometry::get_arc(clamp_d / n);

         for (int i = 1; i <= outer_s; ++i)
         {
            const point ar = geometry::get_arc(i * 1. / n);
            const point br = geometry::get_arc((i - clamp_d) / n);

            point mid = intersect(a, b, ar, br);

            points.emplace_back(mid);
         }

         point top_prev = a;
         point bot_prev = a;

         for (int i = 0; i < points.size(); ++i)
         {
            const point top = points[i];
            const point bot = point(top.x, -top.y);

            from.insert(edge(top_prev, top));
            from.insert(edge(bot_prev, bot));

            top_prev = top;
            bot_prev = bot;
         }

         const transform trf = transform::rotate(2 * PI / n);
         if (clamp_s == di)
         {
            const point midr = top_prev.apply(trf);

            if (is_int)
            {
               from.insert(edge(top_prev, midr));
            }
            else
            {
               const point ar = geometry::get_arc(did / n);
               const point br = geometry::get_arc(-dfrac / n);

               const point c = geometry::get_arc(d / n);

               point cent = intersect(ar, br, a, c);
               from.insert(edge(top_prev, cent));
               from.insert(edge(cent, midr));
            }
         }

         return from;
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
