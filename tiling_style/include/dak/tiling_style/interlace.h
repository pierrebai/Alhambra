#pragma once

#ifndef DAK_TILING_STYLE_INTERLACE_H
#define DAK_TILING_STYLE_INTERLACE_H

#include <dak/tiling_style/outline.h>

#include <dak/geometry/utility.h>

#include <set>

namespace dak
{
   namespace tiling_style
   {
      using geometry::edge_t;
      using geometry::point_t;
      using geometry::polygon_t;

      ////////////////////////////////////////////////////////////////////////////
      //
      // Probably the most important rendering style from an historical point
      // of view.  Interlace assigns an over/under rule to the edges
      // of the map and renders a weave the follows that assignment.  Getting
      // the over/under rule is conceptually simple but difficult in practice,
      // especially since we want to have some robustness against potential
      // degenerate maps.
      //
      // Basically, if a diagram can be interlaced, you can just choose an
      // over/under relationship at one vertex and propagate it to all other
      // vertices using a depth-first search.

      class interlace_t : public outline_t
      {
      public:
         // Parameters of the rendering.
         double shadow_width = 0.05;
         double gap_width = 0.;

         // Creation.
         interlace_t() { }
         interlace_t(const ui::color_t& c) : outline_t(c) { }
         interlace_t(const geometry::edges_map_t& m, const ui::color_t& c) : outline_t(m, c) { }
         interlace_t(const ui::color_t& c, double w) : outline_t(c, w) { }
         interlace_t(const ui::color_t& c, double w, double ow) : outline_t(c, w, ow) { }

         // Copy a layer.
         std::shared_ptr<layer_t> clone() const override;
         void make_similar(const layer_t& other) override;

         // Retrieve a description of this style.
         std::wstring describe() const override;

      protected:
         // The total width including outline and gap.
         double total_width() const { return width + outline_width * 0.5 + gap_width; }

         // Generate the fat lines.
         fat_lines_t generate_fat_lines(bool all_edges) override;

         // Combine fat lines with their twin to have the correct contour at both ends.
         fat_lines_t combine_fat_lines(const fat_lines_t& fat_lines);

         // Get the two before/after points needed to draw the p2 junction
         // of the given edge given the number of connections.
         std::pair<point_t, point_t> get_points_many_connections(const edge_t& an_edge, size_t index, double width, const geometry::edges_map_t::range_t& connections) override;

         // Clear the cache when the map, transform or parameters changes.
         void clear_cache() override;

         // Verify if the cache is valid.
         bool is_cache_invalid() const override;

         // Context containing all data needed to do the over/under weaving.
         //
         // Generated fat lines are guaranteed to be in the same order as
         // the canonical edges.
         //
         // This permits us to easily find the edge associated with each
         // fat line and track which edge have already been processed.
         struct context
         {
            const geometry::edges_map_t::edges_t& edges;
            std::vector<size_t> todos;
            std::vector<bool> done_lines;
         };

         // Propagate over/under weaving.
         void propagate_over_under(context& ctx);

         // Propagate over/under weaving at the intersection at the p1 point of the given edge.
         void propagate_over_under_at_edge_p1(const edge_t& cur_edge, size_t index, context& ctx);

         // Keep a copy of the parameters when the cache was generated to detect when it goes stale.
         double cached_shadow_width = NAN;
         double cached_gap_width = NAN;
         std::vector<bool> is_p1_over;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
