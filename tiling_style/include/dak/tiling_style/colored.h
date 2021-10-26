#pragma once

#ifndef DAK_TILING_STYLE_COLORED_H
#define DAK_TILING_STYLE_COLORED_H

#include <dak/tiling_style/style.h>

#include <dak/ui/color.h>

namespace dak
{
   namespace tiling_style
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // A style that encapsulates drawing a map with a color.

      class colored_t : public style_t
      {
      public:
         // The main color used to draw a style. Some styles derives other colors
         // from this.
         ui::color_t color = ui::color_t::black();

         // Creation.
         colored_t() {}
         colored_t(const ui::color_t& c) : color(c) {}
         colored_t(const geometry::edges_map_t& m) : style_t(m) { }
         colored_t(const geometry::edges_map_t& m, const ui::color_t& c) : style_t(m), color(c) { }

         // Copy a layer.
         void make_similar(const layer_t& other) override;

         // Comparison.
         bool operator==(const layer_t& other) const override;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
