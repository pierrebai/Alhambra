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
      using geometry::edge;
      using geometry::point;
      using geometry::polygon;

      ////////////////////////////////////////////////////////////////////////////
      //
      // Border type used when decribing the borders of the sliver that is drawn
      // over the sliver passing over two edges crossing each other.

      enum class border_type : signed char
      {
         none = 0, erase = 1, black = 2, stretched = 4, backward = 8,
      };

      inline bool operator&(border_type a, border_type b) { return border_type((int)a & (int)b) != border_type::none; }
      inline border_type operator|(border_type a, border_type b) { return border_type((int)a | (int)b); }

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

      class interlace : public outline
      {
      public:
         // Parameters of the rendering.
         double shadow_width = 0.05;

         // Creation.
         interlace() { }
         interlace(const ui::color& c) : outline(c) { }
         interlace(const geometry::map& m, const ui::color& c) : outline(m, c) { }
         interlace(const ui::color& c, double w) : outline(c, w) { }
         interlace(const ui::color& c, double w, double ow) : outline(c, w, ow) { }

         // Copy a layer.
         std::shared_ptr<layer> clone() const override;
         void make_similar(const layer& other) override;

         // Retrieve a description of this style.
         std::wstring describe() const override;

      protected:
         // Generate the fat lines.
         fat_lines generate_fat_lines() override;

         // Information generated about each crossing for each edge.
         struct sliver
         {
            polygon poly;
            std::vector<border_type> borders;
         };
         struct crossing_point
         {
            sliver over;
            sliver under;
         };
         struct crossing_edge
         {
            crossing_point p1_crossing;
            crossing_point p2_crossing;
         };
         typedef std::vector<crossing_edge> crossing_edges;

         // Keep a copy of the parameters when the cache was generated to detect when it goes stale.
         crossing_edges cached_crossing_edges;
         double cached_shadow_width = NAN;

         // Clear the cache when the map, transform or parameters changes.
         void clear_cache() override;

         // Verify if the cache is valid.
         bool is_cache_invalid() const override;

         // An edge we have yet to propagate the over/under weaving to.
         // We keep three piece of data related to each edge:
         //
         //    - the index of the edge in the canonical edge list,
         //    - if the crossing should be over or under,
         //    - which type of edge was at the crossing.
         //
         // That last piece of information is necessary since we find
         // connections between edges via the outbounds edges of their
         // end-point. Those outbound edge are not guaranteed to be
         // canonical, but we can only find canonical edges. So at
         // a few places in the code, we need to make the edge non-
         // canonical to reflect what was found when propagating.
         struct todo
         {
            size_t index = 0;
            bool   is_over = false;
            bool   is_propagation_canonical = true;
         };

         // Context containing all data needed to do the over/under weaving.
         //
         // Generated fat lines are guaranteed to be in the same order as
         // the canonical edges.
         //
         // This permits us to easily find the edge associated with each
         // fat line and track which edge have already been processed.
         struct context
         {
            const geometry::map::edges& edges;
            crossing_edges& crossing_edges;
            std::vector<todo> todos;
            std::vector<bool> done_lines;
            std::set<point> done_slivers;
         };

         // Propagate over/under weaving.
         void propagate_over_under(fat_lines& fat_lines, context& ctx);

         // Propagate over/under weaving at an intersection point.
         // Return true if the crossing point is not a crossing but a continuation of the same line.
         bool propagate_over_under_at_edge_p1(const edge& todo_edge, const bool is_crossing_over, crossing_point& crossing_point, context& ctx);

         void generate_slivers(const size_t index, crossing_point& crossing_point, const std::vector<edge>& connections);

         // Fill the given sliver with the information needed to draw the crossing properly.
         static void fill_sliver(sliver& sliver, const double width, const double extra_width, const size_t index, const std::vector<edge>& connections);

         // Draw the fat lines. Override in-sub-class to change the rendering.
         void internal_draw_fat_lines(ui::drawing& drw, const fat_lines& fat_lines) override;

         // Draw the crossing point between two continuous edges.
         static void draw_crossing_point(ui::drawing& drw, const crossing_point& crossing_point, const ui::color& color, const ui::color& outline_color, const double ow);

         // Draw the borders at a crossing.
         static void internal_draw_borders(ui::drawing& drw, const sliver& sliver, border_type which, bool can_stretch);
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
