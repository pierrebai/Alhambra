#pragma once

#ifndef DAK_TILING_INFER_H
#define DAK_TILING_INFER_H

#include <dak/tiling/mosaic.h>
#include <dak/tiling/tiling.h>
#include <dak/tiling/infer_helpers.h>

#include <dak/geometry/constants.h>
#include <dak/geometry/map.h>
#include <dak/geometry/point.h>
#include <dak/geometry/polygon.h>
#include <dak/geometry/transform.h>

#include <vector>
#include <map>

namespace dak
{
   namespace tiling
   {
      using geometry::map;
      using geometry::point;
      using geometry::polygon;
      using geometry::transform;
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

      class infer
      {
      public:
         const dak::tiling::tiling&       tiling;
         std::map<const polygon, map>     maps;
         std::vector<placed_points>       placed;

         ////////////////////////////////////////////////////////////////////////////
         //
         // Creation. Mosaic must exist as long as the infer object.

         infer(const std::shared_ptr<mosaic>& mo, const polygon& tile);

         ////////////////////////////////////////////////////////////////////////////
         //
         // Building a 3x3 tiling of the prototype.
         //
         // The next three routines create placed_points instances for all
         // the tiles in the nine translational units generated above.

         void add(const transform& trf, const polygon* tile);
         void add(const transform& trf);
         void add(int t1, int t2);

         ////////////////////////////////////////////////////////////////////////////
         //
         // Choose an appropriate transform of the tile to infer, i.e.
         // one that is surrounded by other tiles.  That means that we
         // should just find an instance of that tile in the (0,0) unit.

         const placed_points* findPrimaryFeature(const polygon& tile) const;

         ////////////////////////////////////////////////////////////////////////////
         //
         // For this edge of the tile being inferred, find the edges of
         // neighbouring tiles and store.

         void getAdjacency(const placed_points& pp, const point& main_point, std::vector<adjacency_info>& adjs) const;

         std::vector<adjacency_info> getAdjacencies(const placed_points& pp) const;

         // Take the adjacencies and build a list of contacts by looking at
         // vertices of the maps for the adjacent tiles that lie on the 
         // boundary with the inference region.
         void buildContacts(const placed_points& pp, const std::vector<adjacency_info>& adjs, std::vector<contact>& contacts) const;

         ////////////////////////////////////////////////////////////////////////////
         //
         // Star inferring.

         static std::vector<point> buildStarBranchPoints(
            double d, int s,
            double side_frac, double sign,
            const std::vector<point>& mid_points);

         static map buildStarHalfBranch(
            double d, int s,
            double side_frac, double sign,
            const std::vector<point>& mid_points);

         map inferStar(const polygon& tile, double d, int s);

         ////////////////////////////////////////////////////////////////////////////
         //
         // Girih inferring.

         static point buildGirihHalfBranch(
            const int side, const bool leftBranch,
            const double requiredRotation,
            const std::vector<point>& points, const std::vector<point>& midPoints);

         static intersection_info FindClosestIntersection(
            const int side, const point& sideHalf, const bool isLeftHalf,
            const double requiredRotation,
            const std::vector<point>& points, const std::vector<point>& midPoints);

         static map buildGirihBranch(
            const int side,
            const double requiredRotation,
            const std::vector<point>& points, const std::vector<point>& midPoints);

         map inferGirih(const polygon& tile, int starSides, double starSkip);

         ////////////////////////////////////////////////////////////////////////////
         //
         // Intersect inferring.

         static int getIntersectionRank(int side, bool isLeft, const std::vector<intersection_info>& infos);

         static std::vector<edges_length_info> buildIntersectEdgesLengthInfos(
            const int side, const point& sideHalf, const bool isLeftHalf,
            const double requiredRotation,
            const std::vector<point>& points, const std::vector<point>& midPoints);

         map inferIntersect(const polygon& tile, int starSides, double starSkip, int s);

         ////////////////////////////////////////////////////////////////////////////
         //
         // Progressive intersect inferring.

         static std::vector<intersection_info> buildIntersectionInfos(
            const int side, point sideHalf, const bool isLeftHalf,
            const double requiredRotation,
            const std::vector<point>& points, const std::vector<point>& midPoints);

         map inferIntersectProgressive(const polygon& tile, int starSides, double starSkip, int s);

         ////////////////////////////////////////////////////////////////////////////
         //
         // Hourglass inferring.

         map inferHourglass(const polygon& tile, double d, int s);

         ////////////////////////////////////////////////////////////////////////////
         //
         // Rosette inferring.

         static std::vector<point> buildRosetteBranchPoints(
            double q, int s, double r,
            double side_frac, double sign,
            const std::vector<point>& mid_points_orig,
            const std::vector<point>& corner_points_orig);

         static std::vector<point> buildRosetteIntersections(
            double q, int s, double r,
            double side_frac, double sign,
            const std::vector<point>& mid_points,
            const std::vector<point>& corner_points,
            const std::vector<point>& points);

         static map buildRosetteHalfBranch(
            double q, int s, double r,
            double side_frac, double sign,
            const std::vector<point>& mid_points,
            const std::vector<point>& corner_points);

         map inferRosette(const polygon& tile, double q, int s, double r);

         ////////////////////////////////////////////////////////////////////////////
         //
         // "Normal" magic inferring.

         map simple_infer(const polygon& tile);
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
