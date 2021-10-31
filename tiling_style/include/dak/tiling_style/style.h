#pragma once

#ifndef DAK_TILING_STYLE_STYLE_H
#define DAK_TILING_STYLE_STYLE_H

#include <dak/geometry/edges_map.h>
#include <dak/ui/layer.h>

#include <map>

namespace dak
{
   namespace tiling
   {
      class tiling_t;
      class inflation_tiling_t;
   }

   namespace tiling_style
   {
      using ui::layer_t;
      using geometry::point_t;
      using geometry::edges_map_t;
      using tiling::tiling_t;
      using tiling::inflation_tiling_t;

      ////////////////////////////////////////////////////////////////////////////
      //
      // A style encapsulates drawing a map with some interesting style.

      class style_t : public layer_t
      {
      public:
         // Create an empty style.
         style_t() { }

         // Create a style.
         style_t(const geometry::edges_map_t& m) : my_map(m) { }

         // Retrieve a description of this style.
         virtual std::wstring describe() const = 0;

         // Set or access the map used as the basis to build the style.
         const geometry::edges_map_t& get_map() const { return my_map; }
         virtual void set_map(const geometry::edges_map_t& m, const std::shared_ptr<const tiling_t>& t);

         // Copy a layer.
         void make_similar(const layer_t& other) override;

         // Comparison.
         virtual bool operator==(const layer_t& other) const { return layer_t::operator==(other); }

      protected:
         double get_width_at(const point_t& pt, double width) const;

         void add_inflation_for_point(const point_t& pt, double inflation);

         edges_map_t my_map;

         std::shared_ptr<const inflation_tiling_t> my_tiling;
         point_t my_tiling_center;
         std::map<double, double> my_inflation_by_distances;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
