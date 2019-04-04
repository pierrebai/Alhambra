#pragma once

#ifndef DAK_TILING_TILING_H
#define DAK_TILING_TILING_H

#include <dak/geometry/point.h>
#include <dak/geometry/polygon.h>
#include <dak/geometry/transform.h>

#include <map>
#include <vector>
#include <string>

namespace dak
{
   namespace tiling
   {
      using geometry::point;
      using geometry::polygon;
      using geometry::transform;
      using geometry::rect;

      ////////////////////////////////////////////////////////////////////////////
      //
      // The representation of a tiling, which will serve as the skeleton for
      // Islamic designs.  A tiling has two translation vectors and a set of
      // placed tiles that make up a translational unit.  The idea is that
      // the whole tiling can be replicated across the plane by placing
      // a copy of the translational unit at every integer linear combination
      // of the translation vectors.  In practice, we only draw at those
      // linear combinations within some viewport.

      class tiling
      {
      public:
         // Translations to tile the plane. Two needed for two-dimensional plane.
         // (Of course, more complex tiling exists with rotations and mirrors.)
         // (They are not supported.)
         point t1;
         point t2;

         // The polygonal tiles and where they are placed within the tiling.
         std::map<polygon, std::vector<transform>> tiles;

         // Information about the tiling: name, description and author.
         std::wstring   name;
         std::wstring   description;
         std::wstring   author;

         // Empty tiling.
         tiling();

         // Tiling with a name and translation vectors.
         tiling(const std::wstring& name, const point& t1, const point& t2);

         // Swap two tilings.
         void swap(tiling& other);

         // Tiling comparison.
         bool operator==(const tiling& other) const;
         bool operator!=(const tiling& other) const { return !(*this == other); }

         // Calculate the bounds of the tiles of the tiling.
         rect bounds() const;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 