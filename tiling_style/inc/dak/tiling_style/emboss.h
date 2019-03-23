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

      class emboss : public outline
      {
      public:
         // Angle of the light.
         double angle = 0.;

         // Creation.
         emboss() { }
         emboss(const ui::color& c) : outline(c) { }
         emboss(const geometry::map& m, const ui::color& c) : outline(m, c) { }
         emboss(const ui::color& c, double w) : outline(c, w) { }
         emboss(const ui::color& c, double w, double ow) : outline(c, w, ow) { }

         // Copy a layer.
         std::shared_ptr<layer> clone() const override;
         void make_similar(const layer& other) override;

         // Retrieve a description of this style.
         std::wstring describe() const override;

      protected:
         // The internal draw is called with the layer transform already applied.
         void internal_draw_fat_lines(ui::drawing& drw, const fat_lines& fat_lines) override;

         void draw_trap(ui::drawing& drw, const point& a, const point& b, const point& c, const point& d, const point& light, const ui::color* greys);

         std::pair<point, point> get_points_many_connections(const edge& an_edge, double width, geometry::map::edges& connections) override;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
