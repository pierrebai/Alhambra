#include <dak/tiling_style/outline.h>

#include <dak/geometry/utility.h>

#include <dak/ui/drawing.h>

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
            cached_fat_lines = generate_fat_lines(false);
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

         const ui::stroke outline_stroke = get_stroke(drw, outline_width * 0.5);

         for (const auto& fat_line : fat_lines)
         {
            #ifdef DAK_TILING_STYLE_OUTLINE_RANDOM_COLOR
               drw.set_color(rnd_color.any());
            #else
               drw.set_color(color);
            #endif
            drw.fill_polygon(fat_line.hexagon);

            if (geometry::near_zero(outline_stroke.width))
               continue;

            drw.set_stroke(outline_stroke);
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

      outline::fat_lines outline::generate_fat_lines(bool all_edges)
      {
         fat_lines fat_lines;
         fat_lines.reserve(map.all().size() / (all_edges ? 1 : 2));

         const edge* const first_edge = &*(map.all().begin());

         for (const auto& edge : map.all())
         {
            if (!all_edges && !edge.is_canonical())
               continue;
            const size_t edge_index = &edge - first_edge;
            fat_lines.emplace_back(generate_fat_line(edge, edge_index, width));
         }

         return fat_lines;
      }

      outline::fat_line outline::generate_fat_line(const edge& edge, const size_t edge_index, double width)
      {
         fat_line fat_line;

         const auto tops = get_points(edge,        edge_index, width, fat_line.p2_is_line_end);
         const auto bots = get_points(edge.twin(), edge_index, width, fat_line.p1_is_line_end);

         fat_line.hexagon = polygon({ bots.first, edge.p1, bots.second,
                                      tops.first, edge.p2, tops.second, });

         return fat_line;
      }

      // Look at a given edge and construct a plausible set of points
      // to draw at the edge's 'p2' point.  Call this twice to get the
      // complete outline of the hexagon to draw for this edge.
      std::pair<point, point> outline::get_points(const edge& an_edge, size_t index, double width, bool& is_line_end)
      {
         const geometry::map::range connections = map.outbounds(an_edge.p2);
         const size_t connection_count = connections.size();

         if (connection_count == 1)
         {
            is_line_end = true;
            return get_points_one_connection(an_edge, index, width, connections);
         }
         else
         {
            if (connection_count == 2)
            {
               return get_points_two_connections(an_edge, index, width, connections);
            }
            else
            {
               return get_points_many_connections(an_edge, index, width, connections);
            }

         }
      }

      std::pair<point, point> outline::get_points_one_connection(const edge& an_edge, size_t index, double width, const geometry::map::range&)
      {
         return get_points_dead_end(an_edge, index, width);
      }

      std::pair<point, point> outline::get_points_two_connections(const edge& an_edge, size_t index, double width, const geometry::map::range& connections)
      {
         return get_points_continuation(an_edge, index, width, connections);
      }

      std::pair<point, point> outline::get_points_many_connections(const edge& an_edge, size_t index, double width, const geometry::map::range& connections)
      {
         return get_points_intersection(an_edge, index, width, width, connections);
      }

      std::pair<point, point> outline::get_points_dead_end(const edge& an_edge, size_t index, double width)
      {
         const point dir = (an_edge.p2 - an_edge.p1).normalize();
         const point perp = dir.perp();
         const point below = an_edge.p2 - perp.scale(width);
         const point above = an_edge.p2 + perp.scale(width);
         return std::pair<point, point>(below, above);
      }

      std::pair<point, point> outline::get_points_continuation(const edge& an_edge, size_t index, double width, const geometry::map::range& connections)
      {
         const auto continuation = geometry::map::continuation(connections, an_edge);

         // Note: should not happen, if there is more than one connection there
         //       should be an edge before and an edge after the edge,
         //       but we need to handle failure just in case.
         if (continuation.is_invalid())
            return get_points_dead_end(an_edge, index, width);

         const point jp = get_join(an_edge.p2, an_edge.p1, continuation.p2, width, width);

         if (jp.is_invalid())
            return get_points_dead_end(an_edge, index, width);

         const point below = jp;
         const point above = jp.convex_sum(an_edge.p2, 2.0);

         return std::pair<point, point>(below, above);
      }

      std::pair<point, point> outline::get_points_intersection(const edge& an_edge, size_t index, double width, double other_edges_width, const geometry::map::range& connections)
      {
         const auto before_after = geometry::map::before_after(connections, an_edge);

         // Note: should not happen, if there is more than one connection there
         //       should be an edge before and an edge after the edge,
         //       but we need to handle failure just in case.
         if (before_after.first.is_invalid())
            return get_points_dead_end(an_edge, index, width);

         // TODO: the code below is bad when the width is large and multiple
         //       intersections are nearer to each other than the width.
         //       It results in inversion of polygon points.
         const point dir = (an_edge.p2 - an_edge.p1).normalize();
         const point perp = dir.perp();

         point below = get_join(an_edge.p2, an_edge.p1, before_after.second.p2, width, other_edges_width);
         if (below.is_invalid())
            below = an_edge.p2 - perp.scale(width);

         point above = get_join(an_edge.p2, before_after.first.p2, an_edge.p1, other_edges_width, width);
         if (above.is_invalid())
            above = an_edge.p2 + perp.scale(width);

         return std::pair<point, point>(below, above);
      }

      // Do a mitered join of the two fat lines (a la postscript, for example).
      // The join point on the other side of the joint can be computed by
      // reflecting the point returned by this function through the joint.
      point outline::get_join(const point& joint, const point& a, const point& b, double width_a, double width_b)
      {
         double th = joint.sweep(a, b);

         if (geometry::near(th, 0, 0.01) || geometry::near(th, PI, 0.01) || geometry::near(th, 2 * PI, 0.01))
         {
            return point();
         }
         else
         {
            const point da = (joint - a).normalize();
            const point db = (joint - b).normalize();

            const double la = width_b / std::sin(th);
            const double lb = width_a / std::sin(th);
            const double isx = joint.x - (da.x * la + db.x * lb);
            const double isy = joint.y - (da.y * la + db.y * lb);
            return point(isx, isy);
         }
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
