#pragma once

#ifndef DAK_TILING_MOSAIC_H
#define DAK_TILING_MOSAIC_H

#include <dak/tiling/figure.h>
#include <dak/tiling/tiling.h>

#include <dak/geometry/edges_map.h>
#include <dak/geometry/rectangle.h>

namespace dak
{
   namespace tiling
   {
      using geometry::edges_map_t;
      using geometry::rectangle_t;

      ////////////////////////////////////////////////////////////////////////////
      //
      // The complete information needed to build a mosaic: the tiling and
      // a mapping from tiles to figures.  The mosaic knows how to turn
      // this information into a finished design, returned as a map.

      class mosaic_t
      {
      public:
         // The tiling used to built the mosaic from tiles.
         tiling_t tiling;

         // Figures giving how to draw each tile.
         std::map<polygon_t, std::shared_ptr<figure_t>> tile_figures;

         // Empty mosaic.
         mosaic_t() { }

         // Mosaic of the given tiling, with empty figures.
         mosaic_t(const dak::tiling::tiling_t& t) : tiling(t) { }

         // Copy.
         mosaic_t(const mosaic_t& other);
         mosaic_t& operator=(const mosaic_t& other);
         void swap(mosaic_t& other) noexcept;

         // Mosaic comparison.
         bool operator==(const mosaic_t& other) const;
         bool operator!=(const mosaic_t& other) const { return !(*this == other); }

         // Verify if both mosaics have the same figures.
         bool same_figures(const mosaic_t& other) const;

         // Construct a map in the given polygonal region using the tiling and figures.
         edges_map_t construct(const rectangle_t& region) const;

         // Count how many edge an instance of the tiling requires.
         size_t count_tiling_edges() const;

         // Verify if the mosaic is invalid.
         bool is_invalid() const { return tiling.is_invalid() || tile_figures.empty(); }
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
