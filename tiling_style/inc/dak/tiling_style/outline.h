#pragma once

#ifndef DAK_TILING_STYLE_OUTLINE_H
#define DAK_TILING_STYLE_OUTLINE_H

#include <dak/tiling_style/thick.h>

#include <dak/geometry/polygon.h>

namespace dak
{
   namespace tiling_style
   {
      using geometry::point;
      using geometry::polygon;
      using geometry::edge;

      ////////////////////////////////////////////////////////////////////////////
      //
      // The simplest non-trivial rendering style. Outline just uses
      // some trig to fatten up the map's edges, also drawing a line-based
      // outline for the resulting fat figure.
      //
      // The same code that computes the draw elements for outline can
      // be used by other "fat" styles, such as emboss. Otherwise, using
      // the underlying drawing stroke would be much simpler...

      class outline : public thick
      {
      public:
         // Creation.
         outline() { }
         outline(const ui::color& c) : thick(c) { }
         outline(const geometry::map& m, const ui::color& c) : thick(m, c) { }
         outline(const ui::color& c, double w) : thick(c, w) { }
         outline(const ui::color& c, double w, double ow) : thick(c, w, ow) { }

         // Copy a layer.
         std::shared_ptr<layer> clone() const override;

         // Retrieve a description of this style.
         std::wstring describe() const override;

         // Set the map used as the basis to build the style.
         void set_map(const geometry::map& m) override;

      protected:
         // The internal draw is called with the layer transform already applied.
         void internal_draw(ui::drawing& drw) override;

         // Information about each widened edge.
         struct fat_line
         {
            polygon hexagon;
            bool p1_is_line_end = false;
            bool p2_is_line_end = false;
         };
         typedef std::vector<fat_line> fat_lines;

         // Keep a copy of the parameters when the cache was generated to detect when it goes stale.
         fat_lines cached_fat_lines;
         double cached_width = NAN;
         double cached_outline_width = NAN;

         // Clear the cache when the map, transform or parameters changes.
         virtual void clear_cache();

         // Verify if the cache is valid.
         virtual bool is_cache_invalid() const;

         // Generate the fat lines.
         // The generated fat lines are guaranteed to be in the same order as the canonical edges.
         virtual fat_lines generate_fat_lines();

         // Draw the fat lines. Override in-sub-class to change the rendering.
         virtual void internal_draw_fat_lines(ui::drawing& drw, const fat_lines& fat_lines);

         // Get the two points at the left and right needed to draw the p2 junction
         // of the given edge at the given width.
         std::pair<point, point> get_points(const edge& edge, double width, bool& is_line_end);

         // Get the two before/after points needed to draw the p2 junction
         // of the given edge given the number of connections.
         virtual std::pair<point, point> get_points_one_connection(const edge& an_edge, double width, geometry::map::edges& connections);
         virtual std::pair<point, point> get_points_two_connections(const edge& an_edge, double width, geometry::map::edges& connections);
         virtual std::pair<point, point> get_points_many_connections(const edge& an_edge, double width, geometry::map::edges& connections);

         // Get the two before/after points needed to draw the p2 junction
         // of the given edge when we want to treat the intersection ina specific way.
         std::pair<point, point> get_points_dead_end(const edge& an_edge, double width);
         std::pair<point, point> get_points_continuation(const edge& an_edge, double width, geometry::map::edges& connections);
         std::pair<point, point> get_points_intersection(const edge& an_edge, double width, geometry::map::edges& connections);

         // Calculate the position of a point to draw the junction properly
         // given the angle between and b at the joint.
         static point get_join(const point& joint, const point& a, const point& b, double width);
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
