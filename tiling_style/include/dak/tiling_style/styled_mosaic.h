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
      class rectangle_t;
   }

   namespace tiling_style
   {
      using ui::layer_t;
      using geometry::rectangle_t;

      ////////////////////////////////////////////////////////////////////////////
      //
      // Group a moasic with the draw style in a layer_t.

      class styled_mosaic_t : public layer_t
      {
      public:
         std::shared_ptr<tiling::mosaic_t> mosaic;
         std::shared_ptr<style_t> style;

         // Create an empty mosaic layer_t.
         styled_mosaic_t() { }

         // Copy a layer_t.
         styled_mosaic_t(const styled_mosaic_t&);
         styled_mosaic_t& operator=(const styled_mosaic_t&);

         std::shared_ptr<layer_t> clone() const override;
         void make_similar(const layer_t& other) override;

         // Update the style when the mosaic is modified.
         void update_style(const rectangle_t& region);

      protected:
         // layer_t implementation.
         void internal_draw(ui::drawing_t& drw) override;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
