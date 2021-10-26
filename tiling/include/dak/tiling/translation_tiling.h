#pragma once

#ifndef DAK_TILING_TRANSLATION_TILING_H
#define DAK_TILING_TRANSLATION_TILING_H

#include <dak/tiling/tiling.h>

namespace dak
{
   namespace tiling
   {
      // A translation tiling has two translation vectors.
      //
      // Places copy of the translational unit at every integer linear
      // combination of the translation vectors.  In practice, we only
      // draw at those linear combinations within some viewport.
      class translation_tiling_t : public tiling_t
      {
      public:
         // Translations to tile the plane. Two needed for two-dimensional plane.
         point_t t1;
         point_t t2;

         // Empty tiling.
         translation_tiling_t();

         // Tiling with a name and translation vectors.
         translation_tiling_t(const std::wstring& name) : translation_tiling_t(name, point_t::invalid, point_t::invalid) {}
         translation_tiling_t(const std::wstring& name, const point_t& t1, const point_t& t2);

         // Swap two tilings.
         void swap(translation_tiling_t& other);

         // Tiling comparison.
         bool operator==(const tiling_t& other) const override;
         bool operator!=(const tiling_t& other) const override { return !(*this == other); }

         // Verify if the tiling is invalid.
         bool is_invalid() const override;

         // Fill the given region with copies of the tiling,
         // calling the callback for each transform that place a copy of the tiling.
         void fill(const rectangle_t& region, std::function<void(const tiling_t& tiling, const transform_t& placement)> fill_callback) const override;

         // Count the number of copies of the tiling will be required to fill the given region.
         size_t count_fill_copies(const rectangle_t& region) const override;

         // Fill a region around one copy of the tiling,
         // calling the callback for each transform that place a copy of the tiling.
         void surround(std::function<void(const tiling_t& tiling, const transform_t& placement)> fill_callback) const override;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
