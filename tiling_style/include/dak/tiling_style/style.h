#pragma once

#ifndef DAK_TILING_STYLE_STYLE_H
#define DAK_TILING_STYLE_STYLE_H

#include <dak/geometry/map.h>
#include <dak/ui/layer.h>

namespace dak
{
   namespace tiling_style
   {
      using ui::layer_t;

      ////////////////////////////////////////////////////////////////////////////
      //
      // A style encapsulates drawing a map with some interesting style.

      class style_t : public layer_t
      {
      public:
         // Create an empty style.
         style_t() { }

         // Create a style.
         style_t(const geometry::map& m) : map(m) { }

         // Retrieve a description of this style.
         virtual std::wstring describe() const = 0;

         // Set or access the map used as the basis to build the style.
         const geometry::edges_map_t& get_map() const { return map; }
         virtual void set_map(const geometry::map& m) { map = m; }

         // Copy a layer.
         void make_similar(const layer_t& other) override;

      protected:
         geometry::edges_map_t map;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
