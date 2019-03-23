#pragma once

#ifndef DAK_TILING_STYLE_COLORED_H
#define DAK_TILING_STYLE_COLORED_H

#include <dak/tiling_style/style.h>

#include <dak/ui/drawing.h>

namespace dak
{
   namespace tiling_style
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // A style that encapsulates drawing a map with a color.

      class colored : public style
      {
      public:
         // The main color used to draw a style. Some styles derives other colors
         // from this.
         ui::color color = ui::color::black();

         // Creation.
         colored() {}
         colored(const ui::color& c) : color(c) {}
         colored(const geometry::map& m) : style(m) { }
         colored(const geometry::map& m, const ui::color& c) : style(m), color(c) { }

         // Copy a layer.
         void make_similar(const layer& other) override;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
