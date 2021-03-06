#pragma once

#ifndef DAK_TILING_TILING_SELECTION_H
#define DAK_TILING_TILING_SELECTION_H

#include <dak/tiling/tiling.h>

#include <dak/geometry/edge.h>

#include <dak/utility/selection.h>

#include <memory>

namespace dak
{
   namespace tiling
   {
      using dak::geometry::point_t;
      using dak::geometry::edge_t;
      using dak::geometry::polygon_t;
      using dak::geometry::transform_t;

      using dak::utility::selection_t;

      ////////////////////////////////////////////////////////////////////////////
      //
      // Information kept about each tile in a tiling.

      struct placed_tile_t
      {
         polygon_t tile;
         transform_t trf;

         bool operator==(const placed_tile_t& other) const
         {
            return tile == other.tile && trf == other.trf;
         }
      };

      ////////////////////////////////////////////////////////////////////////////
      //
      // Different types of possible selections in a tiling.

      // Whole tile selection.
      struct tile_selection_t
      {
         std::shared_ptr<placed_tile_t> tile;

         operator const std::shared_ptr<placed_tile_t>& () const
         {
            return tile;
         }

         operator bool() const
         {
            return tile != nullptr;
         }

         operator polygon_t() const
         {
            return tile->tile;
         }
      };

      // Tile edge selection.
      struct edge_selection_t : tile_selection_t
      {
         size_t p1 = -1;
         size_t p2 = -1;

         operator edge_t() const
         {
            if (!tile || p1 < 0 || p2 < 0)
               return edge_t();

            return edge_t(tile->tile.points[p1].apply(tile->trf),
                        tile->tile.points[p2].apply(tile->trf));
         }

         edge_t raw_edge() const
         {
            if (!tile || p1 < 0 || p2 < 0)
               return edge_t();

            return edge_t(tile->tile.points[p1],
                        tile->tile.points[p2]);
         }

         bool operator==(const edge_selection_t& other) const
         {
            return tile == other.tile && p1 == other.p1 && p2 == other.p2;
         }
      };

      // Point selection: single vertex, center of tile or center of an edge.
      struct point_selection_t : tile_selection_t
      {
         std::vector<size_t> points;

         point_selection_t() { }

         point_selection_t(const std::shared_ptr<placed_tile_t>& placed)
         {
            tile = placed;
            if (!placed)
               return;

            for (size_t i = 0; i < placed->tile.points.size(); ++i)
               points.emplace_back(i);
         }

         point_selection_t(const std::shared_ptr<placed_tile_t>& placed, size_t p)
         {
            tile = placed;
            points.emplace_back(p);
         }

         point_selection_t(const std::shared_ptr<placed_tile_t>& placed, size_t p1, size_t p2)
         {
            tile = placed;
            points.emplace_back(p1);
            points.emplace_back(p2);
         }

         operator point_t() const
         {
            if (!tile || points.size() <= 0)
               return point_t();

            point_t pt(0., 0.);
            for (size_t index : points)
               pt = pt + tile->tile.points[index];
            return pt.scale(1. / points.size()).apply(tile->trf);
         }

         bool operator==(const point_selection_t& other) const
         {
            return tile == other.tile && points == other.points;
         }
      };

      ////////////////////////////////////////////////////////////////////////////
      //
      // Combinable selection types.

      enum class selection_type_t
      {
         none     = 0,
         point    = 1,
         edge     = 2,
         tile     = 4,
         edge_or_tile = edge | tile,
         all      = point | edge | tile,
      };

      inline selection_type_t operator|(const selection_type_t s1, const selection_type_t s2)
      {
         return selection_type_t((int)s1 | (int)s2);
      }

      inline selection_type_t operator&(const selection_type_t s1, const selection_type_t s2)
      {
         return selection_type_t((int)s1 & (int)s2);
      }

      inline bool is_selection_single_type(const selection_type_t sel_type)
      {
         switch (sel_type)
         {
         case selection_type_t::none:
         case selection_type_t::point:
         case selection_type_t::edge:
         case selection_type_t::tile:
            return true;
         default:
            return false;
         }
      }

      namespace tiling_selection
      {
         ////////////////////////////////////////////////////////////////////////////
         //
         // Selection finders.
         //
         // The point given must be in world-space, that is in the coordinate system
         // of the tiles.

         selection_t find_selection(std::vector<std::shared_ptr<placed_tile_t>>& tiles, const point_t& wpt, double selection_distance);
         selection_t find_selection(std::vector<std::shared_ptr<placed_tile_t>>& tiles, const point_t& wpt, double selection_distance, const selection_t& excluded_selection);
         selection_t find_selection(std::vector<std::shared_ptr<placed_tile_t>>& tiles, const point_t& wpt, double selection_distance, const selection_t& excluded_selection, selection_type_t);

         ////////////////////////////////////////////////////////////////////////////
         //
         // Selection extractors.

         point_selection_t get_point(const selection_t & sel);
         edge_selection_t  get_edge(const selection_t & sel);
         tile_selection_t  get_placed_tile(const selection_t& sel);
      }

   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
