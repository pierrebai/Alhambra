#pragma once

#ifndef DAK_TILING_MOSAIC_H
#define DAK_TILING_MOSAIC_H

#include <dak/tiling/figure.h>
#include <dak/tiling/tiling.h>

#include <dak/geometry/map.h>
#include <dak/geometry/rect.h>

namespace dak
{
   namespace tiling
   {
      using geometry::map;
      using geometry::rect;

      ////////////////////////////////////////////////////////////////////////////
      //
      // The complete information needed to build a mosaic: the tiling and
      // a mapping from tiles to figures.  The mosaic knows how to turn
      // this information into a finished design, returned as a map.

      class mosaic
      {
      public:
         // The tiling used to built the mosaic from tiles.
         tiling tiling;

         // Figures giving how to draw each tile.
         std::map<polygon, std::shared_ptr<figure>> tile_figures;

         // Empty mosaic.
         mosaic() { }
         
         // Mosaic of the given tiling, with empty figures.
         mosaic(const dak::tiling::tiling& t) : tiling(t) { }

         // Construct a map in the given polygonal region using the tiling and figures.
         map construct(const rect& region) const;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
