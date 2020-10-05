#pragma once

#ifndef DAK_TILING_FIGURE_H
#define DAK_TILING_FIGURE_H

#include <dak/geometry/map.h>

namespace dak
{
   namespace tiling
   {
      using geometry::edges_map_t;

      ////////////////////////////////////////////////////////////////////////////
      //
      // Making the user interface operate directly on maps would be 
      // a hassle.  Maps are a very low level geometry and topological 
      // structure.  Not so good for interacting with users.  So I
      // define a figure class, which is a higher level structure -- 
      // an object that knows how to build maps.  Subclasses of figure
      // understand different ways of bulding maps, but have the advantage
      // of being parameterizable at a high level.

      class figure_t
      {
      public:
         // Construct the map built by the figure.
         const edges_map_t& get_map() const;

         // Copy a figure.
         virtual std::shared_ptr<figure_t> clone() const = 0;
         virtual void make_similar(const figure_t& other) = 0;

         // Verify if this figure is similar to the other:
         // if it's the same type and has the same number of sides.
         virtual bool is_similar(const figure_t& other) const = 0;

         // Comparison.
         virtual bool operator==(const figure_t& other) const = 0;
         bool operator!=(const figure_t& other) const { return !(*this == other); }

         // Retrieve a description of this style.
         virtual std::wstring describe() const = 0;

      protected:
         // Verify if the cached map still corresponds to the current parameters.
         virtual bool is_cache_valid() const = 0;

         // Update the cached parameters.
         virtual void update_cached_values() const = 0;

         // Build the cached map.
         virtual void build_map() const = 0;

         mutable edges_map_t cached_map;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
