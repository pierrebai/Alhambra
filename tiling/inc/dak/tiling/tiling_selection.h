#pragma once

#ifndef DAK_TILING_TILING_SELECTION_H
#define DAK_TILING_TILING_SELECTION_H

#include <dak/tiling/tiling.h>

#include <dak/geometry/edge.h>

#include <dak/utility/selection.h>

namespace dak
{
   namespace tiling
   {
      using dak::geometry::point;
      using dak::geometry::edge;
      using dak::geometry::polygon;
      using dak::geometry::transform;

      using dak::utility::selection;

      ////////////////////////////////////////////////////////////////////////////
      //
      // Information kept about each tile in a tiling.

      struct placed_tile
      {
         polygon tile;
         transform trf;

         bool operator==(const placed_tile& other) const
         {
            return tile == other.tile && trf == other.trf;
         }
      };

      ////////////////////////////////////////////////////////////////////////////
      //
      // Different types of possible selections in a tiling.

      // Whole tile selection.
      struct tile_selection
      {
         std::shared_ptr<placed_tile> tile;

         operator const std::shared_ptr<placed_tile>& () const
         {
            return tile;
         }

         operator bool() const
         {
            return tile != nullptr;
         }

         operator polygon() const
         {
            return tile->tile;
         }
      };

      // Tile edge selection.
      struct edge_selection : tile_selection
      {
         size_t p1 = -1;
         size_t p2 = -1;

         operator edge() const
         {
            if (!tile || p1 < 0 || p2 < 0)
               return edge();

            return edge(tile->tile.points[p1].apply(tile->trf),
                        tile->tile.points[p2].apply(tile->trf));
         }

         edge raw_edge() const
         {
            if (!tile || p1 < 0 || p2 < 0)
               return edge();

            return edge(tile->tile.points[p1],
                        tile->tile.points[p2]);
         }

         bool operator==(const edge_selection& other) const
         {
            return tile == other.tile && p1 == other.p1 && p2 == other.p2;
         }
      };

      // Point selection: single vertex, center of tile or center of an edge.
      struct point_selection : tile_selection
      {
         std::vector<size_t> points;

         point_selection() { }

         point_selection(const std::shared_ptr<placed_tile>& placed)
         {
            tile = placed;
            if (!placed)
               return;

            for (size_t i = 0; i < placed->tile.points.size(); ++i)
               points.emplace_back(i);
         }

         point_selection(const std::shared_ptr<placed_tile>& placed, size_t p)
         {
            tile = placed;
            points.emplace_back(p);
         }

         point_selection(const std::shared_ptr<placed_tile>& placed, size_t p1, size_t p2)
         {
            tile = placed;
            points.emplace_back(p1);
            points.emplace_back(p2);
         }

         operator point() const
         {
            if (!tile || points.size() <= 0)
               return point();

            point pt(0., 0.);
            for (size_t index : points)
               pt = pt + tile->tile.points[index];
            return pt.scale(1. / points.size()).apply(tile->trf);
         }

         bool operator==(const point_selection& other) const
         {
            return tile == other.tile && points == other.points;
         }
      };

      ////////////////////////////////////////////////////////////////////////////
      //
      // Combinable selection types.

      enum class selection_type
      {
         none     = 0,
         point    = 1,
         edge     = 2,
         tile     = 4,
         all      = point | edge | tile,
      };

      inline selection_type operator|(selection_type s1, selection_type s2)
      {
         return selection_type((int)s1 | (int)s2);
      }

      inline selection_type operator&(selection_type s1, selection_type s2)
      {
         return selection_type((int)s1 & (int)s2);
      }

      namespace tiling_selection
      {
         ////////////////////////////////////////////////////////////////////////////
         //
         // Selection finders.
         //
         // The point given must be in world-space, that is in the coordinate system
         // of the tiles.

         selection find_selection(std::vector<std::shared_ptr<placed_tile>>& tiles, const point& wpt, double selection_distance);
         selection find_selection(std::vector<std::shared_ptr<placed_tile>>& tiles, const point& wpt, double selection_distance, const selection& excluded_selection);
         selection find_selection(std::vector<std::shared_ptr<placed_tile>>& tiles, const point& wpt, double selection_distance, const selection& excluded_selection, selection_type);

         ////////////////////////////////////////////////////////////////////////////
         //
         // Selection extractors.

         point_selection get_point(const selection & sel);
         edge_selection  get_edge(const selection & sel);
         tile_selection  get_placed_tile(const selection& sel);
      }

   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
