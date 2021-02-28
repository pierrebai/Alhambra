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

         // Fill a region around one copy of the tiling,
         // calling the callback for each transform that place a copy of the tiling.
         virtual void surround(std::function<void(const tiling_t& tiling, const transform_t& placement)> fill_callback) const = 0;
      };

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

         // Fill a region around one copy of the tiling,
         // calling the callback for each transform that place a copy of the tiling.
         void surround(std::function<void(const tiling_t& tiling, const transform_t& placement)> fill_callback) const override;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
