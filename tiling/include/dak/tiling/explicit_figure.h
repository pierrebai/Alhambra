#pragma once

#ifndef DAK_TILING_EXPLICIT_FIGURE_H
#define DAK_TILING_EXPLICIT_FIGURE_H

#include <dak/tiling/figure.h>

namespace dak
{
   namespace tiling
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // A variety of Figure which contains an explicit map, which is
      // simple returned when the figure is asked for its map.

      class explicit_figure : public figure
      {
      public:
         explicit_figure() { }
         explicit_figure(const map& m) { cached_map = m; }

         // Copy a figure.
         std::shared_ptr<figure> clone() const override;
         void make_similar(const figure&) override { }

         void set_map(const map& m) { cached_map = m; }

         // Retrieve a description of this style.
         std::wstring describe() const override;

         // Figure implementation.
         bool is_similar(const figure&) const override { return false; }

         // Comparison.
         bool operator==(const figure& other) const override;

      protected:
         // Figure cache implementation.
         bool is_cache_valid() const override { return true; }
         void update_cached_values() const override { }
         void build_map() const override { }
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
