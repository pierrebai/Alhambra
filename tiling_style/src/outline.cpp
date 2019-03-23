#include <dak/tiling_style/outline.h>

#include <dak/geometry/utility.h>

#include <cmath>
#include <algorithm>

namespace dak
{
   namespace tiling_style
   {
      using geometry::point;
      using geometry::polygon;
      using geometry::edge;
      using geometry::PI;
      using geometry::is_colinear;

      std::shared_ptr<layer> outline::clone() const
      {
         return std::make_shared<outline>(*this);
      }

      std::wstring outline::describe() const
      {
         return geometry::L::t(L"Outlined");
      }

      void outline::set_map(const geometry::map& m)
      {
         clear_cache();
         thick::set_map(m);
      }

      void outline::internal_draw(ui::drawing& drw)
      {
         if (is_cache_invalid())
         {
            cached_width = width;
            cached_outline_width  = outline_width;
            cached_fat_lines = generate_fat_lines();
         }
         internal_draw_fat_lines(drw, cached_fat_lines);
      }

      void outline::clear_cache()
      {
         cached_fat_lines.clear();
         cached_width = NAN;
         cached_outline_width = NAN;
      }

      bool outline::is_cache_invalid() const
      {
         return cached_fat_lines.size() <= 0
            || cached_width != width
            || cached_outline_width != outline_width;
      }

      void outline::internal_draw_fat_lines(ui::drawing& drw, const fat_lines& fat_lines)
      {
         //#define DAK_TILING_STYLE_OUTLINE_RANDOM_COLOR

         #ifdef DAK_TILING_STYLE_OUTLINE_RANDOM_COLOR
            ui::random_colors rnd_color;
         #endif

         const double ow = drw.get_transform().dist_from_zero(outline_width);

         for (const auto& fat_line : fat_lines)
         {
            #ifdef DAK_TILING_STYLE_OUTLINE_RANDOM_COLOR
               drw.set_color(rnd_color.any());
            #else
               drw.set_color(color);
            #endif
            drw.fill_polygon(fat_line.hexagon);

            if (geometry::near_zero(ow))
               continue;

            drw.set_stroke(ui::stroke(ow));
            drw.set_color(outline_color);

            const auto& pts = fat_line.hexagon.points;
            drw.draw_line(pts[2], pts[3]);
            drw.draw_line(pts[5], pts[0]);

            if (fat_line.p1_is_line_end)
               drw.draw_line(pts[0], pts[2]);

            if (fat_line.p2_is_line_end)
               drw.draw_line(pts[3], pts[5]);
         }
      }

      outline::fat_lines outline::generate_fat_lines()
      {
         fat_lines fat_lines;
         fat_lines.reserve(map.canonicals().size());

         for (const auto& edge : map.canonicals())
         {
            fat_line fat_line;
            const auto top = get_points(edge,        width, fat_line.p2_is_line_end);
            const auto bot = get_points(edge.twin(), width, fat_line.p1_is_line_end);

            fat_line.hexagon = polygon({ bot.first, edge.p1, bot.second,
                                         top.first, edge.p2, top.second, });

            fat_lines.emplace_back(fat_line);
         }

         return fat_lines;
      }

      // Look at a given edge and construct a plausible set of points
      // to draw at the edge's 'p2' point.  Call this twice to get the
      // complete outline of the hexagon to draw for this edge.
      std::pair<point, point> outline::get_points(const edge& an_edge, double width, bool& is_line_end)
      {
         geometry::map::edges connections = map.outbounds(an_edge.p2);
         const size_t connection_count = connections.size();

         if (connection_count == 1)
         {
            is_line_end = true;
            return get_points_one_connection(an_edge, width, connections);
         }
         else
         {
            if (connection_count == 2)
            {
               return get_points_two_connections(an_edge, width, connections);
            }
            else
            {
               return get_points_many_connections(an_edge, width, connections);
            }

         }
      }

      std::pair<point, point> outline::get_points_one_connection(const edge& an_edge, double width, geometry::map::edges&)
      {
         return get_points_dead_end(an_edge, width);
      }

      std::pair<point, point> outline::get_points_two_connections(const edge& an_edge, double width, geometry::map::edges& connections)
      {
         return get_points_continuation(an_edge, width, connections);
      }

      std::pair<point, point> outline::get_points_many_connections(const edge& an_edge, double width, geometry::map::edges& connections)
      {
         return get_points_continuation(an_edge, width, connections);
      }

      std::pair<point, point> outline::get_points_dead_end(const edge& an_edge, double width)
      {
         const point dir = (an_edge.p2 - an_edge.p1).normalize();
         const point perp = dir.perp();
         const point below = an_edge.p2 - perp.scale(width);
         const point above = an_edge.p2 + perp.scale(width);
         return std::pair<point, point>(below, above);
      }

      std::pair<point, point> outline::get_points_continuation(const edge& an_edge, double width, geometry::map::edges& connections)
      {
         const auto continuation = geometry::map::continuation(connections, an_edge);

         // Note: should not happen, if there is more than one connection there
         //       should be an edge before and an edge after the edge,
         //       but we need to handle failure just in case.
         if (continuation.is_invalid())
            return get_points_dead_end(an_edge, width);

         const point jp = get_join(an_edge.p2, an_edge.p1, continuation.p2, width);

         if (jp.is_invalid())
            return get_points_dead_end(an_edge, width);

         const point below = jp;
         const point above = jp.convex_sum(an_edge.p2, 2.0);

         return std::pair<point, point>(below, above);
      }

      std::pair<point, point> outline::get_points_intersection(const edge& an_edge, double width, geometry::map::edges& connections)
      {
         const auto before_after = geometry::map::before_after(connections, an_edge);

         // Note: should not happen, if there is more than one connection there
         //       should be an edge before and an edge after the edge,
         //       but we need to handle failure just in case.
         if (before_after.first.is_invalid())
            return get_points_dead_end(an_edge, width);

         // TODO: the code below is bad when the width is large and multiple
         //       intersections are nearer to each other than the width.
         //       It results in inversion of polygon points.
         const point dir = (an_edge.p2 - an_edge.p1).normalize();
         const point perp = dir.perp();

         point below = get_join(an_edge.p2, an_edge.p1, before_after.second.p2, width);
         if (below.is_invalid())
            below = an_edge.p2 - perp.scale(width);

         point above = get_join(an_edge.p2, before_after.first.p2, an_edge.p1, width);
         if (above.is_invalid())
            above = an_edge.p2 + perp.scale(width);

         return std::pair<point, point>(below, above);
      }

      // Do a mitered join of the two fat lines (a la postscript, for example).
      // The join point on the other side of the joint can be computed by
      // reflecting the point returned by this function through the joint.
      point outline::get_join(const point& joint, const point& a, const point& b, double width)
      {
         double th = joint.sweep(a, b);

         if (geometry::near(th, 0, 0.01) || geometry::near(th, PI, 0.01) || geometry::near(th, 2 * PI, 0.01))
         {
            return point();
         }
         else
         {
            const point d1 = (joint - a).normalize();
            const point d2 = (joint - b).normalize();

            const double l = width / std::sin(th);
            const double isx = joint.x - (d1.x + d2.x) * l;
            const double isy = joint.y - (d1.y + d2.y) * l;
            return point(isx, isy);
         }
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
