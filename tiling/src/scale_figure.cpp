#include <dak/tiling/scale_figure.h>

#include <dak/geometry/constants.h>
#include <dak/geometry/intersect.h>
#include <dak/geometry/transform.h>
#include <dak/geometry/utility.h>

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

      map scale_figure::build_unit() const
      {
         const radial_figure* child = get_child();
         if (!child)
            return map();

         map cunit = child->build_unit();
         const double s = compute_scale();

         if (geometry::near(s, 1.0))
            return cunit;

         // Tip of unit is initially at (1, 0).
         point tip_pos = point::unit_x();

         // Locate the other vertex of the segment we're going to
         // extend.
         // Note: since we ask for outbound edges, the other point is p2.
         point below_tip;
         for (const auto& edge : cunit.outbounds(tip_pos))
         {
            if (edge.p2.y < 0.0)
            {
               below_tip = edge.p2;
               break;
            }
         }

         if (below_tip.is_invalid())
            return cunit;

         // Scale the unit.
         const transform scaling = transform::scale(s);
         cunit.apply_to_self(scaling);
         tip_pos = tip_pos.apply(scaling);
         below_tip = below_tip.apply(scaling);

         // Build the clipping polygon
         std::vector<point> border;
         border.reserve(n);
         for (int i = 0; i < n; ++i)
            border.emplace_back(geometry::get_arc(i * 1. / n));

         // Extend and clip.
         const point seg_end = tip_pos + (tip_pos - below_tip).normalize().scale(100.0);
         point endpoint = tip_pos;
         for (int i = 0; i < n; ++i)
         {
            const point& poly_a = border[i];
            const point& poly_b = border[(i + 1) % n];

            endpoint = intersect(tip_pos, seg_end, poly_a, poly_b);
            if (!endpoint.is_invalid())
               break;
         }

         // Now add the extended edge and its mirror image by first
         // intersecting against rotated versions.
         const transform trf = transform::rotate(2 * PI / n);
         point neg_start = tip_pos.apply(trf);
         point neg_end = point(endpoint.x, -endpoint.y).apply(trf);

         point last_top = tip_pos;
         point last_bottom = tip_pos;

         for (int i = 0; i < (n + 1) / 2; ++i)
         {
            point isect = intersect(tip_pos, endpoint, neg_start, neg_end);
            if (isect.is_invalid())
               break;

            cunit.insert(edge(last_top, isect));
            last_top = isect;
            isect.y = -isect.y;
            cunit.insert(edge(last_bottom, isect));
            last_bottom = isect;

            neg_start = neg_start.apply(trf);
            neg_end = neg_end.apply(trf);
         }

         if (endpoint != last_top)
         {
            cunit.insert(edge(last_top, endpoint));
            endpoint.y = -endpoint.y;
            cunit.insert(edge(last_bottom, endpoint));
         }

         return cunit;
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 

