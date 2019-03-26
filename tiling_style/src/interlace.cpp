#include <dak/tiling_style/interlace.h>

#include <dak/geometry/intersect.h>
#include <dak/geometry/utility.h>

#include <dak/ui/drawing.h>

#include <algorithm>
#include <random>
#include <cmath>

namespace dak
{
   namespace tiling_style
   {
      std::shared_ptr<layer> interlace::clone() const
      {
         return std::make_shared<interlace>(*this);
      }

      void interlace::make_similar(const layer& other)
      {
         outline::make_similar(other);

         if (const interlace* other_interlace = dynamic_cast<const interlace*>(&other))
         {
            shadow_width = other_interlace->shadow_width;
         }
      }

      std::wstring interlace::describe() const
      {
         return geometry::L::t(L"Interlaced");
      }

      void interlace::fill_sliver(sliver& sliver, const double width, const double extra_width, const size_t index, const std::vector<edge>& connections)
      {
         auto& points = sliver.poly.points;
         points.reserve(connections.size() * connections.size());

         const point down = (connections[index % 4].p2 - connections[index % 4].p1).scale(extra_width);
         const point up = (connections[(index + 2) % 4].p2 - connections[(index + 2) % 4].p1).scale(extra_width);

         const border_type e = border_type::erase;
         const border_type n = border_type::none;
         const border_type b = border_type::black;
         const border_type sb = border_type::stretched | border_type::black;
         const border_type se = border_type::stretched | border_type::erase;
         const border_type bb = border_type::backward | border_type::black;
         const border_type be = border_type::backward | border_type::erase;

         const size_t index_offsets_1[] = { 0,  0,  1,  1,  1,  2,  2,  2,  3,  3,  3,  0, };
         const size_t         sides_1[] = { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, };
         const size_t index_offsets_2[] = { 1,  2,  2,  2,  3,  3,  3,  0,  0,  0,  1,  1, };
         const size_t         sides_2[] = { 1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1, };
         const int      displacements[] = { 0,  0,  0,  1,  1,  1,  0,  0,  0, -1, -1, -1, };
         const border_type    borders[] = { bb,  e,  e, sb,  n,  n, bb,  e,  e, sb,  n,  n, };

         for (size_t i = 0; i < sizeof(displacements) / sizeof(displacements[0]); ++i)
         {
            const int e1_index = (index + index_offsets_1[i]) % 4;
            const auto& e1 = connections[e1_index];
            const point e1_perp = (e1.p2 - e1.p1).perp().normalize().scale(width);
            const edge e1_wide_borders[] = { edge(e1.p2 + e1_perp, e1.p1 + e1_perp), edge(e1.p2 - e1_perp, e1.p1 - e1_perp) };

            const int e2_index = (index + index_offsets_2[i]) % 4;
            const auto& e2 = connections[e2_index];
            const point e2_perp = (e2.p2 - e2.p1).perp().normalize().scale(width);
            const edge e2_wide_borders[] = { edge(e2.p2 + e2_perp, e2.p1 + e2_perp), edge(e2.p2 - e2_perp, e2.p1 - e2_perp) };

            const edge& be1 = e1_wide_borders[sides_1[i]];
            const edge& be2 = e2_wide_borders[sides_2[i]];

            point inter = geometry::intersect::intersect_anywhere(be1.p1, be1.p2, be2.p1, be2.p2);
            if (inter.is_invalid())
               continue;

            if (displacements[i] > 0)
               inter = inter + up;
            else if (displacements[i] < 0)
               inter = inter + down;
            points.emplace_back(inter);
            sliver.borders.emplace_back(borders[i]);
         }
      }

      void interlace::generate_slivers(const size_t index_of_over, crossing_point& crossing_point, const std::vector<edge>& connections)
      {
         if (connections.size() < 4)
            return;

         // TODO: multiplier must be relative to crossing angle, I think.
         fill_sliver(crossing_point.over, width, outline_width * 3, index_of_over,     connections);
         fill_sliver(crossing_point.under, width, shadow_width,     index_of_over + 1, connections);
      }

      bool interlace::propagate_over_under_at_edge_p1(const edge& cur_edge, const bool is_crossing_over, crossing_point& crossing_point, context& ctx)
      {
         std::vector<edge> connections = map.outbounds(cur_edge.p1);
         std::sort(connections.begin(), connections.end());
         const size_t connection_count = connections.size();

         const auto todo_iter = std::lower_bound(connections.begin(), connections.end(), cur_edge);
         const size_t cur_index_in_conns = todo_iter - connections.begin();

         const bool is_line_end = (connection_count <= 1);
         if (!is_line_end && !ctx.done_slivers.count(cur_edge.p1))
         {
            ctx.done_slivers.insert(cur_edge.p1);
            const size_t index_of_over = is_crossing_over ? cur_index_in_conns : ((cur_index_in_conns + 1) % connection_count);
            generate_slivers(index_of_over, crossing_point, connections);
         }

         // Propagate the over/under state to all outbound edges. We flip the state
         // at each edge since the weaving makes consecutive edges have opposite
         // over/under state.
         const bool is_not_a_crossing = (connection_count == 2);
         bool next_is_over = is_not_a_crossing ? is_crossing_over : !is_crossing_over;
         for (size_t offset = 1; offset < connection_count; ++offset)
         {
            const size_t next_index_in_conns = (cur_index_in_conns + offset) % connection_count;
            const edge& next_edge = connections[next_index_in_conns];
            const auto next_iter = std::lower_bound(ctx.edges.begin(), ctx.edges.end(), next_edge.canonical());
            const size_t next_index = next_iter - ctx.edges.begin();
            if (!ctx.done_lines[next_index])
            {
               const bool is_propagation_canonical = next_edge.is_canonical();
               ctx.todos.push_back({ next_index, next_is_over, is_propagation_canonical });
            }
            next_is_over = !next_is_over;
         }

         return is_not_a_crossing;
      }

      void interlace::propagate_over_under(fat_lines& fat_lines, context& ctx)
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
            ctx.todos.push_back({ i, false, true });

            while (ctx.todos.size() > 0)
            {
               // Retrieve the new current edge to process and remove it from the stack.
               auto todo = ctx.todos.back();
               ctx.todos.pop_back();

               // If already processed, skip calculations, else mark it as processed.
               if (ctx.done_lines[todo.index])
                  continue;
               else
                  ctx.done_lines[todo.index] = true;

               const edge cur_edge = ctx.edges[todo.index];
               fat_line& cur_fat_line = fat_lines[todo.index];
               crossing_edge& cur_crossing_edge = ctx.crossing_edges[todo.index];

               // Figure which edge to use based on which point (p1 or p2) was found when propagating.
               const edge p1_crossing_edge = todo.is_propagation_canonical ? cur_edge : cur_edge.twin();
               const bool is_p1_crossing_over = todo.is_over;
               const bool is_not_a_crossing = propagate_over_under_at_edge_p1(p1_crossing_edge, is_p1_crossing_over, cur_crossing_edge.p1_crossing, ctx);

               // Note: if the first crossing point we processed turned out to be a continuation, then we don't
               //       flip the over/under flag since the line is continuing.
               const edge p2_crossing_edge = todo.is_propagation_canonical ? cur_edge.twin() : cur_edge;
               const bool is_p2_crossing_over = is_not_a_crossing ? is_p1_crossing_over : !is_p1_crossing_over;
               propagate_over_under_at_edge_p1(p2_crossing_edge, is_p2_crossing_over, cur_crossing_edge.p2_crossing, ctx);
            }
         }
      }

      interlace::fat_lines interlace::generate_fat_lines()
      {
         cached_shadow_width = shadow_width;

         fat_lines fat_lines = outline::generate_fat_lines();

         // Recalculate the over/under propagation if needed.
         cached_crossing_edges.clear();
         context ctx({ map.canonicals(), cached_crossing_edges });
         ctx.done_lines.resize(fat_lines.size(), false);
         ctx.crossing_edges.resize(fat_lines.size());

         propagate_over_under(fat_lines, ctx);

         return fat_lines;
      }

      void interlace::clear_cache()
      {
         outline::clear_cache();
         cached_crossing_edges.clear();
         cached_shadow_width = NAN;
      }
      bool interlace::is_cache_invalid() const
      {
         return outline::is_cache_invalid()
             || cached_shadow_width != shadow_width;
      }

      void interlace::internal_draw_borders(ui::drawing& drw, const sliver& sliver, border_type which, bool can_stretch)
      {
         static constexpr double stretched_scale = 2.5;

         if (sliver.poly.is_invalid())
            return;

         point prev = sliver.poly.points.back();
         for (size_t i = 0; i < sliver.poly.points.size(); ++i)
         {
            const point& pt = sliver.poly.points[i];
            const border_type bt = sliver.borders[i];
            if (bt & which)
            {
               if (can_stretch && bt & border_type::stretched)
               {
                  const point spt = (pt - prev).scale(stretched_scale) + prev;
                  drw.draw_line(prev, spt);
               }
               else if (can_stretch && bt & border_type::backward)
               {
                  const point sprev = (prev - pt).scale(stretched_scale) + pt;
                  drw.draw_line(sprev, pt);
               }
               else
               {
                  drw.draw_line(prev, pt);
               }
            }
            prev = pt;
         }
      }

      void interlace::draw_crossing_point(ui::drawing& drw, const crossing_point& crossing_point, const ui::color& color, const ui::color& outline_color, const double ow)
      {
         if (geometry::near_positive(ow))
         {
            drw.set_color(color);
            internal_draw_borders(drw, crossing_point.under, border_type::erase, false);
         }

         if (!crossing_point.under.poly.is_invalid())
         {
            static constexpr ui::color shadow_color(60, 60, 60, 128);
            drw.set_color(shadow_color);
            drw.fill_polygon(crossing_point.under.poly);
         }

         if (!crossing_point.over.poly.is_invalid())
         {
            drw.set_color(color);
            drw.fill_polygon(crossing_point.over.poly);
         }

         if (geometry::near_positive(ow))
         {
            drw.set_color(outline_color);
            internal_draw_borders(drw, crossing_point.over,  border_type::black | border_type::erase, true);
            internal_draw_borders(drw, crossing_point.under, border_type::black, false);
         }
      };

      void interlace::internal_draw_fat_lines(ui::drawing& drw, const fat_lines& fat_lines)
      {
         outline::internal_draw_fat_lines(drw, fat_lines);

         const ui::stroke outline_stroke = get_stroke(drw, outline_width * 0.5);
         drw.set_stroke(outline_stroke);

         for (const auto& crossing_edge : cached_crossing_edges)
         {
            draw_crossing_point(drw, crossing_edge.p1_crossing, color, outline_color, outline_stroke.width);
            draw_crossing_point(drw, crossing_edge.p2_crossing, color, outline_color, outline_stroke.width);
         }
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
