#pragma once

#ifndef DAK_TILING_STYLE_OUTLINE_H
#define DAK_TILING_STYLE_OUTLINE_H

#include <dak/tiling_style/thick.h>

#include <dak/geometry/polygon.h>

namespace dak
{
   namespace tiling_style
   {
      using geometry::point_t;
      using geometry::polygon_t;
      using geometry::edge_t;

      ////////////////////////////////////////////////////////////////////////////
      //
      // The simplest non-trivial rendering style. Outline just uses
      // some trig to fatten up the map's edges, also drawing a line-based
      // outline for the resulting fat figure.
      //
      // The same code that computes the draw elements for outline can
      // be used by other "fat" styles, such as emboss. Otherwise, using
      // the underlying drawing stroke would be much simpler...

      class outline_t : public thick_t
      {
      public:
         // Creation.
         outline_t() { }
         outline_t(const ui::color_t& c) : thick_t(c) { }
         outline_t(const geometry::edges_map_t& m, const ui::color_t& c) : thick_t(m, c) { }
         outline_t(const ui::color_t& c, double w) : thick_t(c, w) { }
         outline_t(const ui::color_t& c, double w, double ow) : thick_t(c, w, ow) { }

         // Copy a layer.
         std::shared_ptr<layer_t> clone() const override;

         // Retrieve a description of this style.
         std::wstring describe() const override;

         // Set the map used as the basis to build the style.
         void set_map(const geometry::edges_map_t& m, const std::shared_ptr<const tiling_t>& t) override;

      protected:
         // The internal draw is called with the layer transform already applied.
         void internal_draw(ui::drawing_t& drw) override;

         // Information about each widened edge.
         struct fat_line_t
         {
            polygon_t hexagon;
            bool p1_is_line_end = false;
            bool p2_is_line_end = false;
         };
         typedef std::vector<fat_line_t> fat_lines_t;

         // Keep a copy of the parameters when the cache was generated to detect when it goes stale.
         fat_lines_t my_cached_fat_lines;
         double my_cached_width = NAN;
         double my_cached_outline_width = NAN;

         // Clear the cache when the map, transform or parameters changes.
         virtual void clear_cache();

         // Verify if the cache is valid.
         virtual bool is_cache_invalid() const;

         // Generate the fat lines.
         // The generated fat lines are guaranteed to be in the same order as the canonical edges.
         virtual fat_lines_t generate_fat_lines(bool all_edges);

         // Generate one fat line of the edge and width.
         fat_line_t generate_fat_line(const edge_t& edge, const size_t edge_index, double width);

         // Draw the fat lines. Override in-sub-class to change the rendering.
         virtual void internal_draw_fat_lines(ui::drawing_t& drw, const fat_lines_t& fat_lines);

         // Get the two points at the left and right needed to draw the p2 junction
         // of the given edge at the given width.
         std::pair<point_t, point_t> get_points(const edge_t& edge, size_t index, double width, bool& is_line_end);

         // Get the two before/after points needed to draw the p2 junction
         // of the given edge given the number of connections.
         virtual std::pair<point_t, point_t> get_points_one_connection(  const edge_t& an_edge, size_t index, double width, const geometry::edges_map_t::range_t& connections);
         virtual std::pair<point_t, point_t> get_points_two_connections( const edge_t& an_edge, size_t index, double width, const geometry::edges_map_t::range_t& connections);
         virtual std::pair<point_t, point_t> get_points_many_connections(const edge_t& an_edge, size_t index, double width, const geometry::edges_map_t::range_t& connections);

         // Get the two before/after points needed to draw the p2 junction
         // of the given edge when we want to treat the intersection ina specific way.
         std::pair<point_t, point_t> get_points_dead_end(    const edge_t& an_edge, size_t index, double width);
         std::pair<point_t, point_t> get_points_continuation(const edge_t& an_edge, size_t index, double width, const geometry::edges_map_t::range_t& connections);
         std::pair<point_t, point_t> get_points_intersection(const edge_t& an_edge, size_t index, double width, double other_edges_width, const geometry::edges_map_t::range_t& connections);

         // Calculate the position of a point to draw the junction properly
         // given the angle between and b at the joint.
         point_t get_join(const point_t& joint, const point_t& a, const point_t& b, double width, double other_edges_width);
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
