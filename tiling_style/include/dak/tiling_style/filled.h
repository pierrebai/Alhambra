#pragma once

#ifndef DAK_TILING_STYLE_FILLED_H
#define DAK_TILING_STYLE_FILLED_H

#include <dak/tiling_style/colored.h>

#include <dak/geometry/face.h>
#include <dak/geometry/utility.h>

namespace dak
{
   namespace tiling_style
   {
      using geometry::polygon;

      ////////////////////////////////////////////////////////////////////////////
      //
      // A rendering style that converts the map to a collection of 
      // polygonal faces.  The faces are divided into two groups according to
      // a two-colouring of the map (which is always possible for the
      // kinds of Islamic designs we're building).

      class filled_t : public colored_t
      {
      public:
         // Control what gets drawn.
         bool draw_inside = true;
         bool draw_outside = false;

         // Creation.
         filled_t() { }
         filled_t(const ui::color& c) : colored_t(c) {}
         filled_t(const geometry::map& m) : colored_t(m) { }
         filled_t(const geometry::map& m, const ui::color& c) : colored_t(m, c) { }

         // Copy a layer.
         std::shared_ptr<layer> clone() const override;
         void make_similar(const layer& other) override;

         // Retrieve a description of this style.
         std::wstring describe() const override;

         // Set the map used as the basis to build the style.
         void set_map(const geometry::map& m) override;

      protected:
         // The internal draw is called with the layer transform already applied.
         void internal_draw(ui::drawing& drw) override;

         std::vector<polygon> cached_inside;
         std::vector<polygon> cached_outside;
         std::vector<polygon> cached_odd;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
