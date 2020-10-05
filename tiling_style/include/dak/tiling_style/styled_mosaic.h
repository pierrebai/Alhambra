#pragma once

#ifndef DAK_TILING_STYLE_MOSAIC_LAYER_H
#define DAK_TILING_STYLE_MOSAIC_LAYER_H

#include <dak/tiling_style/style.h>

#include <dak/tiling/mosaic.h>
#include <dak/ui/layer.h>

namespace dak
{
   namespace geometry
   {
      class rect;
   }

   namespace tiling_style
   {
      using ui::layer;
      using geometry::rect;

      ////////////////////////////////////////////////////////////////////////////
      //
      // Group a moasic with the draw style in a layer.

      class styled_mosaic : public layer
      {
      public:
         std::shared_ptr<tiling::mosaic> mosaic;
         std::shared_ptr<tiling_style::style> style;

         // Create an empty mosaic layer.
         styled_mosaic() { }

         // Copy a layer.
         styled_mosaic(const styled_mosaic&);
         styled_mosaic& operator=(const styled_mosaic&);

         std::shared_ptr<layer> clone() const override;
         void make_similar(const layer& other) override;

         // Update the style when the mosaic is modified.
         void update_style(const rect& region);

      protected:
         // layer implementation.
         void internal_draw(ui::drawing& drw) override;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
