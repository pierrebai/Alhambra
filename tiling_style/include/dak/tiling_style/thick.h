#pragma once

#ifndef DAK_TILING_STYLE_THICK_H
#define DAK_TILING_STYLE_THICK_H

#include <dak/tiling_style/colored.h>

#include <dak/ui/stroke.h>

namespace dak
{
   namespace ui
   {
      class drawing;
   }

   namespace tiling_style
   {
      using ui::stroke;

      ////////////////////////////////////////////////////////////////////////////
      //
      // A style that has a thickness and can have its outline drawn.

      class thick : public colored
      {
      public:
         // Parameters of the rendering.
         double  width = 0.05;
         double  outline_width = 0.01;
         ui::color outline_color = ui::color::black();
         stroke::join_style join = stroke::join_style::round;

         // Creation.
         thick() { }
         thick(const ui::color& c) : colored(c) { }
         thick(const geometry::map& m, const ui::color& c) : colored(m, c) { }
         thick(const ui::color& c, double w) : colored(c), width(w) { }
         thick(const ui::color& c, double w, double ow) : colored(c), width(w), outline_width(ow) { }

         // Copy a layer.
         std::shared_ptr<layer> clone() const override;
         void make_similar(const layer& other) override;

         // Retrieve a description of this style.
         std::wstring describe() const override;

      protected:
         // The internal draw is called with the layer transform already applied.
         void internal_draw(ui::drawing& drw) override;

         ui::stroke get_stroke(ui::drawing& drw, double width) const;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
