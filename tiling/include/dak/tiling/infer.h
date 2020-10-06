#pragma once

#ifndef DAK_TILING_INFER_H
#define DAK_TILING_INFER_H

#include <dak/tiling/mosaic.h>
#include <dak/tiling/tiling.h>
#include <dak/tiling/infer_helpers.h>

#include <dak/geometry/constants.h>
#include <dak/geometry/edges_map.h>
#include <dak/geometry/point.h>
#include <dak/geometry/polygon.h>
#include <dak/geometry/transform.h>

#include <vector>
#include <map>

namespace dak
{
   namespace tiling
   {
      using geometry::edges_map_t;
      using geometry::point_t;
      using geometry::polygon_t;
      using geometry::transform_t;
      using geometry::PI;
      using geometry::TOLERANCE;
      using geometry::TOLERANCE_2;

      ////////////////////////////////////////////////////////////////////////////
      //
      // Infer is the black magic part of the Islamic design system.  Given 
      // an empty, irregular tile, it infers a map for that tile by 
      // extending line segments from neighbouring tiles.  For many common
      // Islamic designs, this extension is natural and easy to carry out.
      // Of course, because this design tool lets you explore designs that
      // were never drawn in the Islamic tradition, there will be cases where
      // it's not clear what to do.  There will, in fact, be cases where
      // natural extensions of line segments don't exist.  So infer has to
      // have black magic built-in.  It has to use heuristics to construct
      // plausible inferred maps, or at least maps that don't break the rest
      // of the system.
      //
      // If you're using Taprats and experiencing weird behaviour, it's 
      // probably coming from in here.  But you knew what you were infer
      // when you started using Taprats (sorry -- couldn't resist the pun).

      class infer_t
      {
      public:
         const dak::tiling::tiling_t&           tiling;
         std::map<const polygon_t, edges_map_t> maps;
         std::vector<placed_points_t>             placed;

         ////////////////////////////////////////////////////////////////////////////
         //
         // Creation. Mosaic must exist as long as the infer object.

         infer_t(const std::shared_ptr<mosaic_t>& mo, const polygon_t& tile);

         ////////////////////////////////////////////////////////////////////////////
         //
         // Building a 3x3 tiling of the prototype.
         //
         // The next three routines create placed_points_t instances for all
         // the tiles in the nine translational units generated above.

         void add(const transform_t& trf, const polygon_t* tile);
         void add(const transform_t& trf);
         void add(int t1, int t2);

         ////////////////////////////////////////////////////////////////////////////
         //
         // Choose an appropriate transform_t of the tile to infer, i.e.
         // one that is surrounded by other tiles.  That means that we
         // should just find an instance of that tile in the (0,0) unit.

         const placed_points_t* findPrimaryFeature(const polygon_t& tile) const;

         ////////////////////////////////////////////////////////////////////////////
         //
         // For this edge of the tile being inferred, find the edges of
         // neighbouring tiles and store.

         void getAdjacency(const placed_points_t& pp, const point_t& main_point, std::vector<adjacency_info>& adjs) const;

         std::vector<adjacency_info> getAdjacencies(const placed_points_t& pp) const;

         // Take the adjacencies and build a list of contacts by looking at
         // vertices of the maps for the adjacent tiles that lie on the 
         // boundary with the inference region.
         void buildContacts(const placed_points_t& pp, const std::vector<adjacency_info>& adjs, std::vector<contact_t>& contacts) const;

         ////////////////////////////////////////////////////////////////////////////
         //
         // Star inferring.

         static std::vector<point_t> buildStarBranchPoints(
            double d, int s,
            double side_frac, double sign,
            const std::vector<point_t>& mid_points);

         static edges_map_t buildStarHalfBranch(
            double d, int s,
            double side_frac, double sign,
            const std::vector<point_t>& mid_points);

         edges_map_t inferStar(const polygon_t& tile, double d, int s);

         ////////////////////////////////////////////////////////////////////////////
         //
         // Girih inferring.

         static point_t buildGirihHalfBranch(
            const int side, const bool leftBranch,
            const double requiredRotation,
            const std::vector<point_t>& points, const std::vector<point_t>& midPoints);

         static intersection_info FindClosestIntersection(
            const int side, const point_t& sideHalf, const bool isLeftHalf,
            const double requiredRotation,
            const std::vector<point_t>& points, const std::vector<point_t>& midPoints);

         static edges_map_t buildGirihBranch(
            const int side,
            const double requiredRotation,
            const std::vector<point_t>& points, const std::vector<point_t>& midPoints);

         edges_map_t inferGirih(const polygon_t& tile, int starSides, double starSkip);

         ////////////////////////////////////////////////////////////////////////////
         //
         // Intersect inferring.

         static int getIntersectionRank(int side, bool isLeft, const std::vector<intersection_info>& infos);

         static std::vector<edges_length_info_t> buildIntersectEdgesLengthInfos(
            const int side, const point_t& sideHalf, const bool isLeftHalf,
            const double requiredRotation,
            const std::vector<point_t>& points, const std::vector<point_t>& midPoints);

         edges_map_t inferIntersect(const polygon_t& tile, int starSides, double starSkip, int s);

         ////////////////////////////////////////////////////////////////////////////
         //
         // Progressive intersect inferring.

         static std::vector<intersection_info> buildIntersectionInfos(
            const int side, point_t sideHalf, const bool isLeftHalf,
            const double requiredRotation,
            const std::vector<point_t>& points, const std::vector<point_t>& midPoints);

         edges_map_t inferIntersectProgressive(const polygon_t& tile, int starSides, double starSkip, int s);

         ////////////////////////////////////////////////////////////////////////////
         //
         // Hourglass inferring.

         edges_map_t inferHourglass(const polygon_t& tile, double d, int s);

         ////////////////////////////////////////////////////////////////////////////
         //
         // Rosette inferring.

         static std::vector<point_t> buildRosetteBranchPoints(
            double q, int s, double r,
            double side_frac, double sign,
            const std::vector<point_t>& mid_points_orig,
            const std::vector<point_t>& corner_points_orig);

         static std::vector<point_t> buildRosetteIntersections(
            double q, int s, double r,
            double side_frac, double sign,
            const std::vector<point_t>& mid_points,
            const std::vector<point_t>& corner_points,
            const std::vector<point_t>& points);

         static edges_map_t buildRosetteHalfBranch(
            double q, int s, double r,
            double side_frac, double sign,
            const std::vector<point_t>& mid_points,
            const std::vector<point_t>& corner_points);

         edges_map_t inferRosette(const polygon_t& tile, double q, int s, double r);

         ////////////////////////////////////////////////////////////////////////////
         //
         // "Normal" magic inferring.

         edges_map_t simple_infer(const polygon_t& tile);
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
