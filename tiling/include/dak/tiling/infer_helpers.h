#pragma once

#ifndef DAK_TILING_INFER_HELPERS_H
#define DAK_TILING_INFER_HELPERS_H

#include <dak/geometry/constants.h>
#include <dak/geometry/point.h>
#include <dak/geometry/polygon.h>
#include <dak/geometry/transform.h>

#include <vector>

namespace dak
{
   namespace tiling
   {
      using geometry::point;
      using geometry::polygon;
      using geometry::transform;
      using geometry::TOLERANCE_2;

      ////////////////////////////////////////////////////////////////////////////
      //
      // Transformed mid-points of a tile.
      //
      // The transformed midpoints of the tile's edges.
      // Since the tilings are edge to edge, a tile edge can
      // be uniquely identified by its midpoint.  So we can 
      // compare two edges by just comparing their midpoints
      // instead of comparing the endpoints pairwise.

      class placed_points
      {
      public:
         const polygon*       tile;
         transform            trf;
         std::vector<point>   mids;

         placed_points(const polygon* tile, const transform& trf, const std::vector<point>& mids)
         : tile(tile), mids(mids), trf(trf)
         {
         }
      };

      ////////////////////////////////////////////////////////////////////////////
      //
      // Information about what tile and edge on that tile is adjacent
      // to a given edge on a given tile.

      class adjacency_info
      {
      public:
         double            tolerance = 0;
         polygon           tile;
         transform         trf;
         int               edge = -1;

         adjacency_info(const polygon& tile, int edge, const transform& trf, double tolerance)
         : tile(tile), edge(edge), trf(trf), tolerance(tolerance)
         {
         }
      };

      ////////////////////////////////////////////////////////////////////////////
      //
      // Information about intersection of a side left or right in-going edge
      // with a given side right or left edge.

      class intersection_info
      {
      public:
         double   distance_2 = 1e100;  // The square of the distance (square to avoid a square root op.)
         point    intersection;        // The intersection point.
         int      side = -1;           // Which side of the tile this describes.
         int      otherSide = -1;      // Which other side it meets.
         bool     otherIsLeft = false; // True if the other side is the left edge.

         intersection_info() { }

         intersection_info(int side, int otherSide, bool otherIsLeft, double distance_2, point i)
         {
            this->side = side;
            this->otherSide = otherSide;
            this->otherIsLeft = otherIsLeft;
            this->distance_2 = distance_2;
            this->intersection = i;
         }

         bool operator==(const intersection_info& other) const
         {
            const double diff = distance_2 - other.distance_2;
            return diff > -TOLERANCE_2 && diff < TOLERANCE_2;
         }

         bool operator<(const intersection_info& other) const
         {
            return compareTo(other) < 0;
         }

         int compareTo(const intersection_info& other) const
         {
            const double diff = distance_2 - other.distance_2;
            return diff < -TOLERANCE_2 ? -1
                 : diff >  TOLERANCE_2 ? 1
                 : 0;
         }
      };

      ////////////////////////////////////////////////////////////////////////////
      //
      // Information about the length of edges connecting two sides and
      // the intersection point.

      class edges_length_info
      {
      public:
         double   distance_2;       // The square of the distance (square to avoid a square root op.)
         point    intersection;     // The intersection point.
         int      side1;            // Which side of the tile this describes.
         int      side2;            // Which side of the tile this describes.
         int      intersection_count;
         bool     isLeft1;          // True if first side is left edge.
         bool     isLeft2;          // True if second side is left edge.

         edges_length_info(int side1, bool isLeft1, int side2, bool isLeft2, int ic, double distance_2, point i)
         {
            this->side1 = side1;
            this->isLeft1 = isLeft1;
            this->side2 = side2;
            this->isLeft2 = isLeft2;
            this->intersection_count = ic;
            this->distance_2 = distance_2;
            this->intersection = i;
         }

         bool operator==(edges_length_info other) const
         {
            int ic_diff = intersection_count - other.intersection_count;
            if (ic_diff != 0)
               return false;

            const double diff = distance_2 - other.distance_2;
            return diff > -TOLERANCE_2 && diff < TOLERANCE_2;
         }

         bool operator<(edges_length_info other) const
         {
            return compareTo(other) < 0;
         }

         int compareTo(const edges_length_info& other) const
         {
            const double diff = distance_2 - other.distance_2;
            if (diff < -TOLERANCE_2)
               return -1;
            if (diff > TOLERANCE_2)
               return 1;

            const int ic_diff = intersection_count - other.intersection_count;
            if (ic_diff < 0)
               return -1;
            else if (ic_diff > 0)
               return 1;

            return 0;
         }
      };

      ////////////////////////////////////////////////////////////////////////////
      //
      // The information about one point of contact on the boundary of the
      // region being inferred.

      class contact
      {
      public:
         point    position;
         point    other;
         point    end;

         point    isect;
         int      isect_idx = -1;

         static const int COLINEAR_NONE = 0;
         static const int COLINEAR_MASTER = 1;
         static const int COLINEAR_SLAVE = 2;

         int      colinear = COLINEAR_NONE;

         bool     taken = false;

         contact(const point& position, const point& other)
         : position(position), other(other), end(position + (position - other).normalize().scale(10000.0))
         {
         }
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
