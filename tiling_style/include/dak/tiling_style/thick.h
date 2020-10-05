#pragma once

#ifndef DAK_TILING_STYLE_THICK_H
#define DAK_TILING_STYLE_THICK_H

#include <dak/tiling_style/colored.h>

#include <dak/ui/stroke.h>

namespace dak
{
   namespace ui
   {
      class drawing_t;
   }

   namespace tiling_style
   {
      using ui::stroke_t;

      ////////////////////////////////////////////////////////////////////////////
      //
      // A style that has a thickness and can have its outline drawn.

      class thick_t : public colored_t
      {
      public:
         // Parameters of the rendering.
         double  width = 0.05;
         double  outline_width = 0.01;
         ui::color_t outline_color = ui::color_t::black();
         stroke_t::join_style_t join = stroke_t::join_style_t::round;

         // Creation.
         thick_t() { }
         thick_t(const ui::color_t& c) : colored_t(c) { }
         thick_t(const geometry::edges_map_t& m, const ui::color_t& c) : colored_t(m, c) { }
         thick_t(const ui::color_t& c, double w) : colored_t(c), width(w) { }
         thick_t(const ui::color_t& c, double w, double ow) : colored_t(c), width(w), outline_width(ow) { }

         // Copy a layer.
         std::shared_ptr<layer_t> clone() const override;
         void make_similar(const layer_t& other) override;

         // Retrieve a description of this style.
         std::wstring describe() const override;

      protected:
         // The internal draw is called with the layer transform already applied.
         void internal_draw(ui::drawing_t& drw) override;

         ui::stroke_t get_stroke(ui::drawing_t& drw, double width) const;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
