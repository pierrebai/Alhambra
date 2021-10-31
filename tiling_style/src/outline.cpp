#include <dak/tiling_style/outline.h>

#include <dak/geometry/utility.h>

#include <dak/utility/text.h>

#include <dak/ui/drawing.h>

#include <cmath>
#include <algorithm>

namespace dak
{
   namespace tiling_style
   {
      using geometry::point_t;
      using geometry::polygon_t;
      using geometry::edge_t;
      using geometry::PI;
      using geometry::is_colinear;
      using utility::L;

      std::shared_ptr<layer_t> outline_t::clone() const
      {
         return std::make_shared<outline_t>(*this);
      }

      std::wstring outline_t::describe() const
      {
         return L::t(L"Outlined");
      }

      void outline_t::set_map(const geometry::edges_map_t& m, const std::shared_ptr<const tiling_t>& t)
      {
         clear_cache();
         thick_t::set_map(m, t);
      }

      void outline_t::internal_draw(ui::drawing_t& drw)
      {
         if (is_cache_invalid())
         {
            cached_width = width;
            cached_outline_width  = outline_width;
            cached_fat_lines = generate_fat_lines(false);
         }
         internal_draw_fat_lines(drw, cached_fat_lines);
      }

      void outline_t::clear_cache()
      {
         cached_fat_lines.clear();
         cached_width = NAN;
         cached_outline_width = NAN;
      }

      bool outline_t::is_cache_invalid() const
      {
         return cached_fat_lines.size() <= 0
            || cached_width != width
            || cached_outline_width != outline_width;
      }

      void outline_t::internal_draw_fat_lines(ui::drawing_t& drw, const fat_lines_t& fat_lines)
      {
         //#define DAK_TILING_STYLE_OUTLINE_RANDOM_COLOR

         #ifdef DAK_TILING_STYLE_OUTLINE_RANDOM_COLOR
            ui::random_colors rnd_color;
         #endif

         const ui::stroke_t outline_stroke = get_stroke(drw, outline_width);

         for (const auto& fat_line : fat_lines)
         {
            #ifdef DAK_TILING_STYLE_OUTLINE_RANDOM_COLOR
               auto c = rnd_color.any();
               c.a = 120;
               drw.set_color(c);
            #else
               drw.set_color(color);
            #endif
            drw.fill_polygon(fat_line.hexagon);

            if (utility::near_zero(outline_stroke.width))
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

      outline_t::fat_lines_t outline_t::generate_fat_lines(bool all_edges)
      {
         fat_lines_t fat_lines;
         fat_lines.reserve(map.all().size() / (all_edges ? 1 : 2));

         const edge_t* const first_edge = &*(map.all().begin());

         for (const auto& edge : map.all())
         {
            if (!all_edges && !edge.is_canonical())
               continue;
            const size_t edge_index = &edge - first_edge;
            fat_lines.emplace_back(generate_fat_line(edge, edge_index, width));
         }

         return fat_lines;
      }

      outline_t::fat_line_t outline_t::generate_fat_line(const edge_t& edge, const size_t edge_index, double width)
      {
         fat_line_t fat_line;

         const auto tops = get_points(edge,        edge_index, width, fat_line.p2_is_line_end);
         const auto bots = get_points(edge.twin(), edge_index, width, fat_line.p1_is_line_end);

         fat_line.hexagon = polygon_t({ bots.first, edge.p1, bots.second,
                                        tops.first, edge.p2, tops.second, });

         return fat_line;
      }

      // Look at a given edge and construct a plausible set of points
      // to draw at the edge's 'p2' point.  Call this twice to get the
      // complete outline of the hexagon to draw for this edge.
      std::pair<point_t, point_t> outline_t::get_points(const edge_t& an_edge, size_t index, double width, bool& is_line_end)
      {
         const geometry::edges_map_t::range_t connections = map.outbounds(an_edge.p2);
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

      std::pair<point_t, point_t> outline_t::get_points_one_connection(const edge_t& an_edge, size_t index, double width, const geometry::edges_map_t::range_t&)
      {
         return get_points_dead_end(an_edge, index, width);
      }

      std::pair<point_t, point_t> outline_t::get_points_two_connections(const edge_t& an_edge, size_t index, double width, const geometry::edges_map_t::range_t& connections)
      {
         return get_points_continuation(an_edge, index, width, connections);
      }

      std::pair<point_t, point_t> outline_t::get_points_many_connections(const edge_t& an_edge, size_t index, double width, const geometry::edges_map_t::range_t& connections)
      {
         return get_points_intersection(an_edge, index, width, width, connections);
      }

      std::pair<point_t, point_t> outline_t::get_points_dead_end(const edge_t& an_edge, size_t index, double width)
      {
         const point_t dir = (an_edge.p2 - an_edge.p1).normalize();
         const point_t perp = dir.perp();
         const point_t below = an_edge.p2 - perp.scale(get_width_at(an_edge.p2, width));
         const point_t above = an_edge.p2 + perp.scale(get_width_at(an_edge.p2, width));
         return std::pair<point_t, point_t>(below, above);
      }

      std::pair<point_t, point_t> outline_t::get_points_continuation(const edge_t& an_edge, size_t index, double width, const geometry::edges_map_t::range_t& connections)
      {
         const auto continuation = geometry::edges_map_t::continuation(connections, an_edge);

         // Note: should not happen, if there is more than one connection there
         //       should be an edge before and an edge after the edge,
         //       but we need to handle failure just in case.
         if (continuation.is_invalid())
            return get_points_dead_end(an_edge, index, width);

         const point_t jp = get_join(an_edge.p2, an_edge.p1, continuation.p2, width, width);

         if (jp.is_invalid())
            return get_points_dead_end(an_edge, index, width);

         const point_t below = jp;
         const point_t above = jp.convex_sum(an_edge.p2, 2.0);

         return std::pair<point_t, point_t>(below, above);
      }

      std::pair<point_t, point_t> outline_t::get_points_intersection(const edge_t& an_edge, size_t index, double width, double other_edges_width, const geometry::edges_map_t::range_t& connections)
      {
         const auto before_after = geometry::edges_map_t::before_after(connections, an_edge);

         // Note: should not happen, if there is more than one connection there
         //       should be an edge before and an edge after the edge,
         //       but we need to handle failure just in case.
         if (before_after.first.is_invalid())
            return get_points_dead_end(an_edge, index, width);

         // TODO: the code below is bad when the width is large and multiple
         //       intersections are nearer to each other than the width.
         //       It results in inversion of polygon points.
         point_t dir = (an_edge.p2 - an_edge.p1);
         const double dist = dir.mag();
         dir = dir.scale(1.0 / dist);
         const point_t perp = dir.perp();

         width = std::min(width, dist);
         other_edges_width = std::min(other_edges_width, dist);

         point_t below = get_join(an_edge.p2, an_edge.p1, before_after.second.p2, width, other_edges_width);
         if (below.is_invalid())
            below = an_edge.p2 - perp.scale(get_width_at(an_edge.p2, width));

         point_t above = get_join(an_edge.p2, before_after.first.p2, an_edge.p1, other_edges_width, width);
         if (above.is_invalid())
            above = an_edge.p2 + perp.scale(get_width_at(an_edge.p2, width));

         return std::pair<point_t, point_t>(below, above);
      }

      // Do a mitered join of the two fat lines (a la postscript, for example).
      // The join point on the other side of the joint can be computed by
      // reflecting the point returned by this function through the joint.
      point_t outline_t::get_join(const point_t& joint, const point_t& a, const point_t& b, double width_a, double width_b)
      {
         double th = joint.sweep(a, b);

         if (utility::near(th, 0, 0.01) || utility::near(th, PI, 0.01) || utility::near(th, 2 * PI, 0.01))
         {
            return point_t();
         }
         else
         {
            const point_t da = (joint - a).normalize();
            const point_t db = (joint - b).normalize();

            width_a = get_width_at(a, width_a);
            width_b = get_width_at(b, width_b);

            const double sth = std::sin(th);
            const double la = width_b / sth;
            const double lb = width_a / sth;
            const double isx = joint.x - (da.x * la + db.x * lb);
            const double isy = joint.y - (da.y * la + db.y * lb);
            return point_t(isx, isy);
         }
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
