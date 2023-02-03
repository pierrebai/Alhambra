#include <dak/tiling_style/interlace.h>

#include <dak/geometry/intersect.h>

#include <dak/utility/text.h>

#include <dak/ui/drawing.h>

#include <algorithm>
#include <random>
#include <cmath>
#include <set>

namespace dak
{
   namespace tiling_style
   {
      using namespace geometry::intersect;
      using utility::L;

      std::shared_ptr<layer_t> interlace_t::clone() const
      {
         return std::make_shared<interlace_t>(*this);
      }

      void interlace_t::make_similar(const layer_t& other)
      {
         outline_t::make_similar(other);

         if (const interlace_t* other_interlace = dynamic_cast<const interlace_t*>(&other))
         {
            shadow_width = other_interlace->shadow_width;
            gap_width = other_interlace->gap_width;
         }
      }

      bool interlace_t::operator==(const layer_t& other) const
      {
         if (!outline_t::operator==(other))
            return false;

         if (const interlace_t* other_interlace = dynamic_cast<const interlace_t*>(&other))
         {
            return dak::utility::near(shadow_width, other_interlace->shadow_width)
                && dak::utility::near(gap_width, other_interlace->gap_width);
         }

         return false;
      }

      std::wstring interlace_t::describe() const
      {
         return L::t(L"Interlaced");
      }

      void interlace_t::propagate_over_under_at_edge_p1(const edge_t& cur_edge, size_t index, over_under_context_t& ctx)
      {
         const geometry::edges_map_t::range_t connections = my_map.outbounds(cur_edge.p1);
         const size_t connection_count = connections.size();
         const edge_t* conn_edges = &*(connections.begin());

         const auto todo_iter = std::lower_bound(connections.begin(), connections.end(), cur_edge);
         const size_t cur_index_in_conns = todo_iter - connections.begin();

         const bool is_crossing_over = my_is_p1_over[index];

         const bool is_not_a_crossing = (connection_count == 2);

         // Propagate to its twin.
         const bool twin_is_over = is_not_a_crossing ? is_crossing_over : !is_crossing_over;
         const edge_t twin_edge = cur_edge.twin();
         const auto twin_iter = std::lower_bound(ctx.edges.begin(), ctx.edges.end(), twin_edge);
         const size_t twin_index = twin_iter - ctx.edges.begin();
         if (!ctx.done_lines[twin_index])
         {
            my_is_p1_over[twin_index] = twin_is_over;
            ctx.todos.push_back(twin_index);
         }

         // Propagate the over/under state to all outbound edges. We flip the state
         // at each edge since the weaving makes consecutive edges have opposite
         // over/under state.
         bool next_is_over = is_not_a_crossing ? is_crossing_over : !is_crossing_over;
         for (size_t offset = 1; offset < connection_count; ++offset)
         {
            const size_t next_index_in_conns = (cur_index_in_conns + offset) % connection_count;
            const edge_t& next_edge = conn_edges[next_index_in_conns];
            const auto next_iter = std::lower_bound(ctx.edges.begin(), ctx.edges.end(), next_edge);
            const size_t next_index = next_iter - ctx.edges.begin();
            if (!ctx.done_lines[next_index])
            {
               my_is_p1_over[next_index] = next_is_over;
               ctx.todos.push_back(next_index);
            }
            next_is_over = !next_is_over;
         }
      }

      void interlace_t::propagate_over_under(over_under_context_t& ctx)
      {
         // Propagate over/under weaving at the intersection of the given edge p1.

         for (size_t i = 0; i < ctx.done_lines.size(); ++i)
         {
            if (ctx.done_lines[i])
               continue;

            // We start a new batch of edges, can happen because there are
            // isolated islands of edges. The over/under state of the first
            // edge is arbitraty; we start at false, because it's better if
            // line-ends are under.
            ctx.todos.push_back(i);

            while (ctx.todos.size() > 0)
            {
               // Retrieve the new current edge to process and remove it from the stack.
               size_t index_todo = ctx.todos.back();
               ctx.todos.pop_back();

               // If already processed, skip calculations, else mark it as processed.
               if (ctx.done_lines[index_todo])
                  continue;
               else
                  ctx.done_lines[index_todo] = true;

               // Propagate at the p1 of the edge to all neighbour and its twin.
               propagate_over_under_at_edge_p1(ctx.edges[index_todo], index_todo, ctx);
            }
         }
      }

      interlace_t::fat_lines_t interlace_t::generate_fat_lines(bool all_edges)
      {
         my_cached_shadow_width = shadow_width;
         my_cached_gap_width = gap_width;

         // Recalculate the over/under propagation.
         over_under_context_t ctx({ my_map.all() });
         ctx.done_lines.resize(ctx.edges.size(), false);
         my_is_p1_over.resize(ctx.edges.size(), false);
         propagate_over_under(ctx);

         all_edges = true;
         fat_lines_t fat_lines = outline_t::generate_fat_lines(all_edges);

         fat_lines = combine_fat_lines(fat_lines);

         fat_lines = combine_continuations(fat_lines);

         // TODO: shadow.

         return fat_lines;
      }

      interlace_t::fat_lines_t interlace_t::combine_continuations(const interlace_t::fat_lines_t& fat_lines)
      {
         interlace_t::fat_lines_t combined;
         combined.reserve(fat_lines.size() / 2 + 1);

         std::set<edge_t> already_done;

         const auto& edges = my_map.all();
         for (size_t edge_index = 0; edge_index < edges.size(); ++edge_index)
         {
            const edge_t& edge = edges[edge_index];
            if (!edge.is_canonical())
               continue;

            if (already_done.count(edge))
               continue;

            const edge_t twin = edge.twin();
            const size_t twin_index = std::lower_bound(edges.begin(), edges.end(), twin) - edges.begin();

            if (my_is_p1_over[edge_index])
            {
               const edge_t& continuation_edge = my_map.continuation(twin);
               if (continuation_edge.is_invalid())
                  continue;

               const edge_t other_edge = continuation_edge.is_canonical()
                  ? continuation_edge
                  : continuation_edge.twin();
               if (already_done.count(other_edge))
                  continue;

               const double angle = edge.angle(other_edge);
               if (!utility::near(angle, 0.0) && !utility::near(angle, geometry::PI))
                  continue;

               // Combine the two edges contour.
               const size_t other_index = std::lower_bound(edges.begin(), edges.end(), other_edge) - edges.begin();

               // Use the edge fat-line as the basis.
               combined.emplace_back(fat_lines[edge_index]);
               auto& fat_line = combined.back();

               if (continuation_edge.is_canonical())
               {
                  fat_line.hexagon.points[0] = fat_lines[other_index].hexagon.points[5];
                  fat_line.hexagon.points[1] = fat_lines[other_index].hexagon.points[4];
                  fat_line.hexagon.points[2] = fat_lines[other_index].hexagon.points[3];
               }
               else
               {
                  fat_line.hexagon.points[0] = fat_lines[other_index].hexagon.points[0];
                  fat_line.hexagon.points[1] = fat_lines[other_index].hexagon.points[1];
                  fat_line.hexagon.points[2] = fat_lines[other_index].hexagon.points[2];
               }

               already_done.insert(edge);
               already_done.insert(other_edge);
            }
            else if (my_is_p1_over[twin_index])
            {
               const edge_t& continuation_edge = my_map.continuation(edge);
               if (continuation_edge.is_invalid())
                  continue;

               const edge_t other_edge = continuation_edge.is_canonical()
                  ? continuation_edge
                  : continuation_edge.twin();
               if (already_done.count(other_edge))
                  continue;

               const double angle = edge.angle(other_edge);
               if (!utility::near(angle, 0.0) && !utility::near(angle, geometry::PI))
                  continue;

               // Combine the two edges contour.
               const size_t other_index = std::lower_bound(edges.begin(), edges.end(), other_edge) - edges.begin();

               // Use the edge fat-line as the basis.
               combined.emplace_back(fat_lines[edge_index]);
               auto& fat_line = combined.back();

               if (continuation_edge.is_canonical())
               {
                  fat_line.hexagon.points[3] = fat_lines[other_index].hexagon.points[3];
                  fat_line.hexagon.points[4] = fat_lines[other_index].hexagon.points[4];
                  fat_line.hexagon.points[5] = fat_lines[other_index].hexagon.points[5];
               }
               else
               {
                  fat_line.hexagon.points[3] = fat_lines[other_index].hexagon.points[2];
                  fat_line.hexagon.points[4] = fat_lines[other_index].hexagon.points[1];
                  fat_line.hexagon.points[5] = fat_lines[other_index].hexagon.points[0];
               }

               already_done.insert(edge);
               already_done.insert(other_edge);
            }
         }

         for (size_t edge_index = 0; edge_index < edges.size(); ++edge_index)
         {
            const edge_t& edge = edges[edge_index];
            if (!edge.is_canonical())
               continue;

            if (already_done.count(edge))
               continue;

            combined.emplace_back(fat_lines[edge_index]);
         }

         return combined;
      }


      interlace_t::fat_lines_t interlace_t::combine_fat_lines(const interlace_t::fat_lines_t& fat_lines)
      {
         interlace_t::fat_lines_t combined;
         combined.reserve(fat_lines.size());

         const auto& edges = my_map.all();
         for (size_t edge_index = 0; edge_index < edges.size(); ++edge_index)
         {
            const auto& edge = edges[edge_index];
            combined.emplace_back(fat_lines[edge_index]);
            if (!edge.is_canonical())
               continue;

            // Use the edge fat-line as the basis.
            auto& fat_line = combined.back();

            // Add the twin contour for the other end.
            const auto twin = edge.twin();
            const size_t twin_index = std::lower_bound(edges.begin(), edges.end(), twin) - edges.begin();

            fat_line.hexagon.points[3] = fat_lines[twin_index].hexagon.points[0];
            fat_line.hexagon.points[4] = fat_lines[twin_index].hexagon.points[1];
            fat_line.hexagon.points[5] = fat_lines[twin_index].hexagon.points[2];

            // Adjust mid-points to be properly placed.
            //
            // We find the outer point of the intersecting edge and if it falls between the other two points
            // we consider its position valid. Otherwise we use the mid-point of the other two-points.
            //
            // Do this for each end of the hexagon.

            const double max_width = std::min(total_width(), edge.p1.distance(edge.p2));

            if (!my_is_p1_over[edge_index] && my_map.outbounds(edge.p1).size() > 2)
            {
               const auto& intersecting_edge = my_map.before(twin);
               const size_t intersecting_edge_index = std::lower_bound(edges.begin(), edges.end(), intersecting_edge) - edges.begin();
               const auto intersecting_edge_outer_points = get_points_continuation(intersecting_edge.twin(), intersecting_edge_index, max_width, my_map.outbounds(edge.p1));
               const double proj_on_line = intersecting_edge_outer_points.first.parameterization_on_line(fat_line.hexagon.points[0], fat_line.hexagon.points[2]);
               if (utility::near_greater_or_equal(proj_on_line, 0.) && utility::near_less_or_equal(proj_on_line, 1.))
                  fat_line.hexagon.points[1] = intersecting_edge_outer_points.first;
               else
                  fat_line.hexagon.points[1] = fat_line.hexagon.points[0].convex_sum(fat_line.hexagon.points[2], 0.5);
            }

            if (!my_is_p1_over[twin_index] && my_map.outbounds(edge.p2).size() > 2)
            {
               const auto& intersecting_edge = my_map.after(edge);
               const size_t intersecting_edge_index = std::lower_bound(edges.begin(), edges.end(), intersecting_edge) - edges.begin();
               const auto intersecting_edge_outer_points = get_points_continuation(intersecting_edge.twin(), intersecting_edge_index, max_width, my_map.outbounds(edge.p2));
               const double proj_on_line = intersecting_edge_outer_points.second.parameterization_on_line(fat_line.hexagon.points[3], fat_line.hexagon.points[5]);
               if (utility::near_greater_or_equal(proj_on_line, 0.) && utility::near_less_or_equal(proj_on_line, 1.))
                  fat_line.hexagon.points[4] = intersecting_edge_outer_points.second;
               else
                  fat_line.hexagon.points[4] = fat_line.hexagon.points[3].convex_sum(fat_line.hexagon.points[5], 0.5);
            }
         }

         return combined;
      }

      std::pair<point_t, point_t> interlace_t::get_points_many_connections(const edge_t& an_edge, size_t index, double width, const geometry::edges_map_t::range_t& connections)
      {
         if (my_is_p1_over[index])
            return get_points_continuation(an_edge, index, width, connections);
         else
            // TODO: sometimes this moves the line outside the normal path (outward widening when meeting a corner).
            return get_points_intersection(an_edge, index, width, total_width(), connections);
      }

      void interlace_t::clear_cache()
      {
         outline_t::clear_cache();
         my_cached_shadow_width = NAN;
         my_cached_gap_width = NAN;
      }
      bool interlace_t::is_cache_invalid() const
      {
         return outline_t::is_cache_invalid()
             || my_cached_shadow_width != shadow_width
             || my_cached_gap_width != gap_width;
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
