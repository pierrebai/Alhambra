#include <dak/tiling_style/interlace.h>

#include <dak/geometry/intersect.h>

#include <dak/utility/text.h>

#include <dak/ui/drawing.h>

#include <algorithm>
#include <random>
#include <cmath>

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

      std::wstring interlace_t::describe() const
      {
         return L::t(L"Interlaced");
      }

      void interlace_t::propagate_over_under_at_edge_p1(const edge_t& cur_edge, size_t index, context& ctx)
      {
         const geometry::edges_map_t::range_t connections = map.outbounds(cur_edge.p1);
         const size_t connection_count = connections.size();
         const edge_t* conn_edges = &*(connections.begin());

         const auto todo_iter = std::lower_bound(connections.begin(), connections.end(), cur_edge);
         const size_t cur_index_in_conns = todo_iter - connections.begin();

         const bool is_crossing_over = is_p1_over[index];

         const bool is_not_a_crossing = (connection_count == 2);

         // Propagate to its twin.
         const bool twin_is_over = is_not_a_crossing ? is_crossing_over : !is_crossing_over;
         const edge_t twin_edge = cur_edge.twin();
         const auto twin_iter = std::lower_bound(ctx.edges.begin(), ctx.edges.end(), twin_edge);
         const size_t twin_index = twin_iter - ctx.edges.begin();
         if (!ctx.done_lines[twin_index])
         {
            is_p1_over[twin_index] = twin_is_over;
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
               is_p1_over[next_index] = next_is_over;
               ctx.todos.push_back(next_index);
            }
            next_is_over = !next_is_over;
         }
      }

      void interlace_t::propagate_over_under(context& ctx)
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
         cached_shadow_width = shadow_width;
         cached_gap_width = gap_width;

         // Recalculate the over/under propagation.
         context ctx({ map.all() });
         ctx.done_lines.resize(ctx.edges.size(), false);
         is_p1_over.resize(ctx.edges.size(), false);
         propagate_over_under(ctx);

         all_edges = true;
         fat_lines_t fat_lines = outline_t::generate_fat_lines(all_edges);

         fat_lines = combine_fat_lines(fat_lines);

         // TODO: shadow.

         return fat_lines;
      }

      interlace_t::fat_lines_t interlace_t::combine_fat_lines(const interlace_t::fat_lines_t& fat_lines)
      {
         interlace_t::fat_lines_t combined;

         const auto& edges = map.all();
         for (size_t edge_index = 0; edge_index < edges.size(); ++edge_index)
         {
            const auto& edge = edges[edge_index];
            if (!edge.is_canonical())
               continue;

            // Use the edge fat-line as the basis.
            combined.emplace_back(fat_lines[edge_index]);
            auto& fat_line = combined.back();

            // Add the twin contour for the other end.
            const auto twin = edge.twin();
            const size_t twin_index = std::lower_bound(edges.begin(), edges.end(), twin) - edges.begin();

            fat_line.hexagon.points[3] = fat_lines[twin_index].hexagon.points[0];
            fat_line.hexagon.points[4] = fat_lines[twin_index].hexagon.points[1];
            fat_line.hexagon.points[5] = fat_lines[twin_index].hexagon.points[2];

            // Adjust mid-points to be properly placed.

            if (!is_p1_over[edge_index] && map.outbounds(edge.p1).size() > 2)
            {
               const auto& before = map.before(twin);
               const size_t before_index = std::lower_bound(edges.begin(), edges.end(), before) - edges.begin();
               const auto end1 = get_points_continuation(before.twin(), before_index, total_width(), map.outbounds(edge.p1));
               const double proj_on_line = end1.first.parameterization_on_line(fat_line.hexagon.points[0], fat_line.hexagon.points[2]);
               if (utility::near_greater_or_equal(proj_on_line, 0.) && utility::near_less_or_equal(proj_on_line, 1.))
                  fat_line.hexagon.points[1] = end1.first;
               else
                  fat_line.hexagon.points[1] = fat_line.hexagon.points[0].convex_sum(fat_line.hexagon.points[2], 0.5);
            }

            if (!is_p1_over[twin_index] && map.outbounds(edge.p2).size() > 2)
            {
               const auto& after = map.after(edge);
               const size_t after_index = std::lower_bound(edges.begin(), edges.end(), after) - edges.begin();
               const auto end4 = get_points_continuation(after.twin(), after_index, total_width(), map.outbounds(edge.p2));
               const double proj_on_line = end4.second.parameterization_on_line(fat_line.hexagon.points[3], fat_line.hexagon.points[5]);
               if (utility::near_greater_or_equal(proj_on_line, 0.) && utility::near_less_or_equal(proj_on_line, 1.))
                  fat_line.hexagon.points[4] = end4.second;
               else
                  fat_line.hexagon.points[4] = fat_line.hexagon.points[3].convex_sum(fat_line.hexagon.points[5], 0.5);
            }
         }

         return combined;
      }

      std::pair<point_t, point_t> interlace_t::get_points_many_connections(const edge_t& an_edge, size_t index, double width, const geometry::edges_map_t::range_t& connections)
      {
         if (is_p1_over[index])
            return get_points_continuation(an_edge, index, width, connections);
         else
            // TODO: sometimes this moves the line outside the normal path (outward widening when meeting a corner).
            return get_points_intersection(an_edge, index, width, total_width(), connections);
      }

      void interlace_t::clear_cache()
      {
         outline_t::clear_cache();
         cached_shadow_width = NAN;
         cached_gap_width = NAN;
      }
      bool interlace_t::is_cache_invalid() const
      {
         return outline_t::is_cache_invalid()
             || cached_shadow_width != shadow_width
             || cached_gap_width != gap_width;
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
