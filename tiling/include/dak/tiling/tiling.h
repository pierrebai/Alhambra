#pragma once

#ifndef DAK_TILING_TILING_H
#define DAK_TILING_TILING_H

#include <dak/geometry/point.h>
#include <dak/geometry/polygon.h>
#include <dak/geometry/transform.h>

#include <map>
#include <vector>
#include <string>
#include <functional>

namespace dak
{
   namespace tiling
   {
      using geometry::point_t;
      using geometry::polygon_t;
      using geometry::transform_t;
      using geometry::rectangle_t;

      ////////////////////////////////////////////////////////////////////////////
      //
      // The common representation of a tiling, which will serve as the skeleton
      // for Islamic designs. A tiling has a set of placed tiles that make up
      // a base tiling unit.
      //
      // The idea is that the base tiling unit can be replicated across the plane
      // by following a recipe for the placement of each copies.
      //
      // The sub-classes of tiling_t provide the fill() function which does this
      // replication process for their particular duplication recipe.

      class tiling_t
      {
      public:
         // The polygonal tiles and where they are placed within the tiling.
         // This form the tiling base unit.
         std::map<polygon_t, std::vector<transform_t>> tiles;

         // Information about the tiling: name, description and author.
         std::wstring   name;
         std::wstring   description;
         std::wstring   author;

         // Empty tiling.
         tiling_t();

         // Tiling with a name.
         tiling_t(const std::wstring& name);

         // Swap two tilings.
         void swap(tiling_t& other);

         // Tiling comparison.
         virtual bool operator==(const tiling_t& other) const;
         virtual bool operator!=(const tiling_t& other) const { return !(*this == other); }

         // Calculate the bounds of the polygonal tiles of the tiling.
         rectangle_t bounds() const;

         // Verify if the tiling is invalid.
         virtual bool is_invalid() const;

         // Fill the given region with copies of the tiling,
         // calling the callback for each transform that place a copy of the tiling.
         virtual void fill(const rectangle_t& region, std::function<void(const tiling_t& tiling, const transform_t& placement)> fill_callback) const = 0;

         // Count the number of copies of the tiling will be required to fill the given region.
         virtual size_t count_fill_copies(const rectangle_t& region) const = 0;

         // Fill a region around one copy of the tiling,
         // calling the callback for each transform that place a copy of the tiling.
         virtual void surround(std::function<void(const tiling_t& tiling, const transform_t& placement)> fill_callback) const = 0;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
