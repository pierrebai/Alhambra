#pragma once

#ifndef DAK_TILING_STYLE_EMBOSS_H
#define DAK_TILING_STYLE_EMBOSS_H

#include <dak/tiling_style/outline.h>

#include <dak/geometry/utility.h>

#include <cmath>

namespace dak
{
   namespace tiling_style
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // A rendering style for maps that pretends that the map is carves out
      // of wood with a diamond cross section and shines a directional light 
      // on the wood from some angle.  The map is drawn as a collection of 
      // trapezoids, and the darkness of each trapezoid is controlled by the
      // angle the trapezoid makes with the light source.  The result is a 
      // simple but highly effective 3D effect, similar to flat-shaded 3D
      // rendering.
      //
      // In practice, we can make this a subclass of outline -- it uses the
      // same pre-computed fat line array, and just add one parameter and
      // overloads the draw fat-lines function.

      class emboss_t : public outline_t
      {
      public:
         // Angle of the light.
         double angle = 0.;

         // Creation.
         emboss_t() { }
         emboss_t(const ui::color_t& c) : outline_t(c) { }
         emboss_t(const geometry::edges_map_t& m, const ui::color_t& c) : outline_t(m, c) { }
         emboss_t(const ui::color_t& c, double w) : outline_t(c, w) { }
         emboss_t(const ui::color_t& c, double w, double ow) : outline_t(c, w, ow) { }

         // Copy a layer.
         std::shared_ptr<layer_t> clone() const override;
         void make_similar(const layer_t& other) override;

         // Retrieve a description of this style.
         std::wstring describe() const override;

      protected:
         // The internal draw is called with the layer transform already applied.
         void internal_draw_fat_lines(ui::drawing_t& drw, const fat_lines_t& fat_lines) override;

         void draw_trap(ui::drawing_t& drw, const point_t& a, const point_t& b, const point_t& c, const point_t& d, const point_t& light, const ui::color_t* greys);

         std::pair<point_t, point_t> get_points_many_connections(const edge_t& an_edge, size_t index, double width, const geometry::edges_map_t::range_t& connections) override;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
