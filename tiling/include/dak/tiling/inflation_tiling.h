#pragma once

#ifndef DAK_TILING_INFLATION_TILING_H
#define DAK_TILING_INFLATION_TILING_H

#include <dak/tiling/tiling.h>

#include <dak/geometry/edge.h>

namespace dak
{
   namespace tiling
   {
      using geometry::edge_t;

      // A inflation tiling has two scaling vectors meeting at the inflation center
      // and a scaling factor to create outward rings.
      //
      // Places copy of the tiling unit at every multiple of the circle fraction, around
      // the inflation center. It also places scaled versions both outward and inward
      // of the initial ring of tiling units.

      class inflation_tiling_t : public tiling_t
      {
      public:
         // Translations to tile the plane by scaling.
         edge_t s1;
         edge_t s2;
         transform_t inflation;

         // Empty tiling.
         inflation_tiling_t();

         // Tiling with a name.
         inflation_tiling_t(const std::wstring& name, const edge_t& s1, const edge_t& s2, const transform_t& inflation);

         // Swap two tilings.
         void swap(inflation_tiling_t& other);

         // Tiling comparison.
         bool operator==(const tiling_t& other) const override;
         bool operator!=(const tiling_t& other) const override { return !(*this == other); }

         // Verify if the tiling is invalid.
         bool is_invalid() const override;

         // Calculate the tiling center fro the translation edges.
         point_t get_center() const;

         // Fill the given region with copies of the tiling,
         // calling the callback for each transform that place a copy of the tiling.
         void fill(const rectangle_t& region, std::function<void(const tiling_t& tiling, const transform_t& placement)> fill_callback) const override;

         // Count the number of copies of the tiling will be required to fill the given region.
         size_t count_fill_copies(const rectangle_t& region) const override;

         // Fill a region around one copy of the tiling,
         // calling the callback for each transform that place a copy of the tiling.
         void surround(std::function<void(const tiling_t& tiling, const transform_t& placement)> fill_callback) const override;

         // Fill the given region with copies of the tiling,
         // calling the callback for each transform that place a copy of the tiling.
         void fill_rings(int rings_count, std::function<void(const tiling_t& tiling, const transform_t& placement)> fill_callback) const;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
