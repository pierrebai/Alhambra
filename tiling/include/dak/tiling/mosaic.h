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

         // Copy.
         mosaic(const mosaic& other);
         mosaic& operator=(const mosaic& other);
         void swap(mosaic& other) noexcept;

         // Mosaic comparison.
         bool operator==(const mosaic& other) const;
         bool operator!=(const mosaic& other) const { return !(*this == other); }

         // Verify if both mosaics have the same figures.
         bool same_figures(const mosaic& other) const;

         // Construct a map in the given polygonal region using the tiling and figures.
         map construct(const rect& region) const;

         // Count how many edge an instance of the tiling requires.
         size_t count_tiling_edges() const;

         // Verify if the mosaic is invalid.
         bool is_invalid() const { return tiling.is_invalid() || tile_figures.empty(); }
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
