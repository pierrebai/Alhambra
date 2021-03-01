#include <dak/tiling/infer.h>
#include <dak/tiling/infer_mode.h>
#include <dak/tiling/irregular_figure.h>

#include <dak/geometry/edge.h>
#include <dak/geometry/intersect.h>
#include <dak/geometry/utility.h>

#include <algorithm>
#include <iterator>

namespace dak
{
   namespace tiling
   {
      using geometry::edge_t;
      using namespace geometry::intersect;

      const wchar_t* infer_mode_name(infer_mode_t m)
      {
         switch (m)
         {
            case infer_mode_t::star:
               return L"Star";
            case infer_mode_t::girih:
               return L"Girih";
            case infer_mode_t::intersect:
               return L"Intersect";
            case infer_mode_t::progressive:
               return L"Progessive";
            case infer_mode_t::hourglass:
               return L"Hourglass";
            case infer_mode_t::rosette:
               return L"Rosette";
            case infer_mode_t::extended_rosette:
               return L"Extended Rosette";
            case infer_mode_t::simple:
               return L"Simple";
            default:
               return L"Unknown";
         }
      }

      infer_mode_t infer_mode_from_name(const wchar_t* name)
      {
         if (!name)
            return infer_mode_t::girih;

         return infer_mode_from_name(std::wstring(name));
      }

      infer_mode_t infer_mode_from_name(const std::wstring& name)
      {
         const infer_mode_t modes[] =
         {
            infer_mode_t::star,
            infer_mode_t::girih,
            infer_mode_t::intersect,
            infer_mode_t::progressive,
            infer_mode_t::hourglass,
            infer_mode_t::rosette,
            infer_mode_t::extended_rosette,
            infer_mode_t::simple,
         };

         for (const auto mode : modes)
            if (std::wstring(infer_mode_name(mode)) == name)
               return mode;

         // Some reasonable default...
         return infer_mode_t::girih;
      }

      namespace
      {
         ////////////////////////////////////////////////////////////////////////////
         //
         // Retrieving container size as an integer to avoid warnings.

         template<class CONTAINER>
         int length(const CONTAINER& container)
         {
            return int(container.size());
         }

         ////////////////////////////////////////////////////////////////////////////
         //
         // The different kinds of connections that can be made between
         // contacts, in increasing order of badness.  This is used to 
         // compare two possible connections.

         static const int INSIDE_EVEN = 0;
         static const int INSIDE_COLINEAR = 1;
         static const int INSIDE_UNEVEN = 2;
         static const int OUTSIDE_EVEN = 3;
         static const int OUTSIDE_UNEVEN = 4;
         static const int NONE = 5;

         int lexCompareDistances(int kind1, double dist1, int kind2, double distance_2)
         {
            if (kind1 < kind2)
               return -1;
            else if (kind1 > kind2)
               return 1;

            if (utility::near(dist1, distance_2))
               return 0;
            else if (dist1 < distance_2)
               return -1;

            return 1;
         }

         ////////////////////////////////////////////////////////////////////////////
         //
         // Pseudo points around a circle inscribed in the figure, like those for
         // regular radial figures. Of course, the figure being ierrgular, we
         // instead interpolate betwwen mid-points.
         //
         // TODO: use bezier interpolation instead of linear.

         point_t getArc(double frac, const std::vector<point_t>& pts)
         {
            while (frac > 1.0)
               frac -= 1.0;
            while (frac < 0.0)
               frac += 1.0;
            int indexPrev = ((int) std::floor(length(pts) * frac + 0.01)) % length(pts);
            int indexNext = ((int) std::ceil(length(pts) * frac - 0.01)) % length(pts);
            const point_t& prev = pts[indexPrev];
            const point_t& next = pts[indexNext];

            double pts_frac = length(pts) * frac - indexPrev;
            return prev.convex_sum(next, pts_frac);
         }
      }

      infer_t::infer_t(const std::shared_ptr<mosaic_t>& mo, const polygon_t& tile)
         : tiling(mo->tiling)
      {
         // Get a edges map for each tile in the prototype.
         for (const auto& tf : mo->tile_figures)
         {
            //if (tf.first != tile)
            {
               // We have to use the cached maps for irregulr to avoid infinite recursion.
               if (auto irregular = std::dynamic_pointer_cast<irregular_figure_t>(tf.second))
                  maps[tf.first] = irregular->cached_map;
               else
                  maps[tf.first] = tf.second->get_map();
            }
         }

         // I'm going to generate all the tiles in the translational units
         // (x,y) where -1 <= x, y <= 1.  This is guaranteed to surround
         // every tile in the (0,0) unit by tiles.  You can then get
         // a sense of what other tiles surround a tile on every edge.
         tiling->surround([self = this](const tiling_t&, const transform_t& placement) {
            self->add(placement);
         });
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // Building a 3x3 tiling of the prototype.
      //
      // The next three routines create placed_points_t instances for all
      // the tiles in the nine translational units generated above.

      void infer_t::add(const transform_t& trf, const polygon_t* tile)
      {
         int sz = length(tile->points);
         std::vector<point_t> fpts = geometry::apply(trf, tile->points);
         std::vector<point_t> mids;
         for (int idx = 0; idx < sz; ++idx)
            mids.emplace_back(fpts[idx].convex_sum(fpts[(idx + 1) % sz], 0.5));

         placed.emplace_back(placed_points_t(tile, trf, mids));
      }

      void infer_t::add(const transform_t& base_trf)
      {
         for (const auto& placed : tiling->tiles)
            for (const auto& trf : placed.second)
               add(base_trf.compose(trf), &placed.first);
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // Choose an appropriate transform of the tile to infer, i.e.
      // one that is surrounded by other tiles.  That means that we
      // should just find an instance of that tile in the (0,0) unit.

      const placed_points_t* infer_t::findPrimaryFeature(const polygon_t& tile) const
      {
         // The start and end of the tiles in the (0,0) unit.
         auto iter = std::next(placed.begin(), length(placed) * 4 / 9);
         const auto end = std::next(placed.begin(), length(placed) * 5 / 9);
         int cur_reg_count = -1;
         const placed_points_t* cur = nullptr;

         for (; iter < end; ++iter)
         {
            const placed_points_t& pp = *iter;

            if (*pp.tile != tile)
               continue;

            // Count the number of regular tiles surrounding this one,
            // in the case a tile is not always surrounded by the same
            // tiles, we want to select the one with the most regulars.
            std::vector<adjacency_info> adjs = getAdjacencies(pp);
            if (adjs.size() <= 0)
               continue;

            int new_reg_count = 0;
            for (const auto& adj : adjs)
               if (adj.tile.is_regular())
                  new_reg_count++;

            if (new_reg_count > cur_reg_count)
            {
               cur_reg_count = new_reg_count;
               cur = &pp;
            }
         }

         return cur;
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // For this edge of the tile being inferred, find the edges of
      // neighbouring tiles and store.

      void infer_t::getAdjacency(const placed_points_t& pp, const point_t& main_point, std::vector<adjacency_info>& adjs) const
      {
         for (double tolerance = TOLERANCE; tolerance < 5.0; tolerance *= 2)
         {
            for (const auto& pcur : placed)
            {
               if (&pcur == &pp)
                  continue;

               for (int ov = 0; ov < length(pcur.mids); ++ov)
               {
                  if (near(pcur.mids[ov], main_point, tolerance))
                  {
                     adjs.emplace_back(*pcur.tile, ov, pcur.trf, tolerance);
                     return;
                  }
               }
            }
         }
      }

      std::vector<adjacency_info> infer_t::getAdjacencies(const placed_points_t& pp) const
      {
         std::vector<adjacency_info> ret;

         for (const auto& mid : pp.mids)
            getAdjacency(pp, mid, ret);

         return ret;
      }


      // Take the adjacencies and build a list of contacts by looking at
      // vertices of the maps for the adjacent tiles that lie on the 
      // boundary with the inference region.
      void infer_t::buildContacts(const placed_points_t& pp, const std::vector<adjacency_info>& adjs, std::vector<contact_t>& contacts) const
      {
         std::vector<point_t> fpts = geometry::apply(pp.trf, pp.tile->points);

         // Get the transformed edges map for each adjacent tile.  I'm surprised
         // at how fast this ends up being!
         std::vector<edges_map_t> amaps;
         for (int idx = 0; idx < length(adjs); ++idx)
         {
            if (adjs[idx].tile.is_invalid())
            {
               amaps.emplace_back();
            }
            else
            {
               const auto iter = maps.find(adjs[idx].tile);
               if (iter == maps.end())
               {
                  amaps.emplace_back();
               }
               else
               {
                  const edges_map_t& fig = iter->second;
                  amaps.emplace_back(fig.apply(adjs[idx].trf));
               }
            }
         }

         // Now, for each edge in the transformed tile, find a (hopefully
         // _the_) vertex in the adjacent edges map that lies on the edge.  When
         // a vertex is found, all (hopefully _both_) the edges incident 
         // on that vertex are added as contacts.

         for (int idx = 0; idx < length(fpts); ++idx)
         {
            const point_t& a = fpts[idx];
            const point_t& b = fpts[(idx + 1) % length(fpts)];

            const edges_map_t& edges_map_t = amaps[idx];
            const adjacency_info& adj = adjs[idx];

            for (const auto& v : edges_map_t.all())
            {
               if (!v.is_canonical())
                  continue;
               const point_t& pos = v.p1;
               const double distance_2 = pos.distance_2_to_line(a, b);
               if (utility::near_zero(distance_2, adj.tolerance)
                   && !geometry::near(pos, a, adj.tolerance)
                   && !geometry::near(pos, b, adj.tolerance))
               {
                  // This vertex lies on the edge.  Add all its edges
                  // to the contact list.
                  for (const auto& edge_t : edges_map_t.outbounds(pos))
                  {
                     contacts.emplace_back(contact_t(pos, edge_t.p2));
                  }
               }
            }
         }
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // Star inferring.

      std::vector<point_t> infer_t::buildStarBranchPoints(
         double d, int s,
         double side_frac, double sign,
         const std::vector<point_t>& mid_points)
      {
         int side_count = length(mid_points);
         double circle_frac = 1. / side_count;

         double clamp_d = std::max(1.0, std::min(d, 0.5 * side_count - 0.01));

         int d_i = (int) std::floor(clamp_d + 0.01);
         double d_frac = clamp_d - d_i;

         s = std::min(s, d_i);
         int outer_s = std::min(s, d_i - 1);

         if (utility::near_zero(d_frac))
         {
            d_frac = 0.0;
         }
         else if (utility::near_zero(1.0 - d_frac))
         {
            d_frac = 0.0;
            d_i += 1;
         }

         point_t a = getArc(side_frac, mid_points);
         point_t b = getArc(side_frac + sign * clamp_d * circle_frac, mid_points);

         std::vector<point_t> points;
         points.emplace_back(a);

         for (int idx = 1; idx <= outer_s; ++idx)
         {
            point_t ar = getArc(side_frac + sign * idx             * circle_frac, mid_points);
            point_t br = getArc(side_frac + sign * (idx - clamp_d) * circle_frac, mid_points);
            point_t intersection = intersect(a, b, ar, br);
            // FIXME: we should handle the concave case by extending the intersection.
            //        (After all, two lines will intersect if not parallel and two
            //         consecutive edges can hardly be parallel.)
            if (!intersection.is_invalid())
               points.emplace_back(intersection);
         }

         return points;
      }

      edges_map_t infer_t::buildStarHalfBranch(
         double d, int s,
         double side_frac, double sign,
         const std::vector<point_t>& mid_points)
      {
         edges_map_t edges_map_t;

         std::vector<point_t> points = buildStarBranchPoints(d, s, side_frac, sign, mid_points);

         for (int idx = 1; idx < length(points); ++idx)
            edges_map_t.insert(edge_t(points[idx - 1], points[idx]));

         int side_count = length(mid_points);
         double circle_frac = 1. / side_count;
         double clamp_d = std::max(1.0, std::min(d, 0.5 * side_count - 0.01));
         int d_i = (int) std::floor(clamp_d + 0.01);
         double d_frac = clamp_d - d_i;
         s = std::min(s, d_i);

         if (s == d_i && sign > 0)
         {
            std::vector<point_t> next_branch_points = buildStarBranchPoints(d, s, side_frac + sign * circle_frac, sign, mid_points);
            const point_t& midr = next_branch_points[length(next_branch_points) - 1];

            if (d_frac == 0.0)
            {
               edges_map_t.insert(edge_t(points.back(), midr));
            }
            else
            {
               point_t ar = getArc(side_frac + sign * d_i    * circle_frac, mid_points);
               point_t br = getArc(side_frac - sign * d_frac * circle_frac, mid_points);
               point_t c = getArc(side_frac + sign * d * circle_frac, mid_points);
               point_t cent = intersect(ar, br, points[0], c);
               if (!cent.is_invalid())
               {
                  edges_map_t.insert(edge_t(points.back(), cent));
                  edges_map_t.insert(edge_t(cent, midr));
               }
            }
         }

         return edges_map_t;
      }

      edges_map_t infer_t::inferStar(const polygon_t& tile, double d, int s)
      {
         edges_map_t infer_map;

         // Get the index of a good transform for this tile.
         const placed_points_t* pmain = findPrimaryFeature(tile);
         if (!pmain)
            return infer_map;

         const std::vector<point_t>& mid_points = pmain->mids;

         int side_count = length(mid_points);
         for (int side = 0; side < side_count; ++side)
         {
            double side_frac = (double) side / (double) side_count;

            infer_map.merge(buildStarHalfBranch(d, s, side_frac, 1.0, mid_points));
            infer_map.merge(buildStarHalfBranch(d, s, side_frac, -1.0, mid_points));
         }

         infer_map.apply_to_self(pmain->trf.invert());

         return infer_map;
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // Girih inferring.

      point_t infer_t::buildGirihHalfBranch(
         const int side, const bool leftBranch,
         const double requiredRotation,
         const std::vector<point_t>& points, const std::vector<point_t>& midPoints)
      {
         const int side_count = length(points);
         const int next = (side + 1) % side_count;
         transform_t rot = transform_t::rotate(midPoints[side],
                                           leftBranch ? -requiredRotation : requiredRotation);
         point_t halfBranch = points[leftBranch ? side : next].apply(rot);

         halfBranch = halfBranch - midPoints[side];
         halfBranch = halfBranch.scale(32); // TODO: magic number!!!
         halfBranch = halfBranch + midPoints[side];

         return halfBranch;
      }

      intersection_info infer_t::FindClosestIntersection(
         const int side, const point_t& sideHalf, const bool isLeftHalf,
         const double requiredRotation,
         const std::vector<point_t>& points, const std::vector<point_t>& midPoints)
      {
         intersection_info info;

         const point_t& sideMidPoint = midPoints[side];

         const bool otherIsLeft = !isLeftHalf;

         const int side_count = length(points);
         for (int i_side = 0; i_side < side_count; ++i_side)
         {
            if (i_side == side)
               continue;

            const point_t& otherMidPoint = midPoints[i_side];

            point_t intersection;
            if (utility::near_zero(otherMidPoint.distance_2_to_line(sideMidPoint, sideHalf)))
            {
               // Edge meets directly the other mid-points, so the distance is the shortest possible.
               intersection = otherMidPoint.convex_sum(sideMidPoint, 0.5);
            }
            else
            {
               point_t otherSide = buildGirihHalfBranch(i_side, otherIsLeft, requiredRotation, points, midPoints);
               intersection = intersect(otherMidPoint, otherSide, sideMidPoint, sideHalf);
            }

            if (intersection.is_invalid())
               continue;
            double distance_2 = intersection.distance_2(sideMidPoint) + intersection.distance_2(otherMidPoint);
            if (utility::near(distance_2, info.distance_2)) {
               // In case of absolute equality, we use the nearest side.
               if (std::abs(side - i_side) < std::abs(side - info.side))
               {
                  info.side = i_side;
                  info.distance_2 = distance_2;
                  info.intersection = intersection;
               }
            }
            if (distance_2 < info.distance_2)
            {
               info.side = i_side;
               info.distance_2 = distance_2;
               info.intersection = intersection;
            }
         }

         return info;
      }

      edges_map_t infer_t::buildGirihBranch(
         const int side,
         const double requiredRotation,
         const std::vector<point_t>& points, const std::vector<point_t>& midPoints)
      {
         edges_map_t infer_map;

         // Find which other edge will intersect this one first.
         for (int i_halves = 0; i_halves < 2; ++i_halves)
         {
            bool isLeftHalf = (i_halves == 0);

            // Find an intersection, if any.
            point_t sideHalf = buildGirihHalfBranch(side, isLeftHalf, requiredRotation, points, midPoints);
            intersection_info info = FindClosestIntersection(
               side, sideHalf, isLeftHalf, requiredRotation, points, midPoints);
            if (info.intersection.is_invalid())
               continue;

            // Mid-point of this side is always included in the edges map.
            infer_map.insert(edge_t(midPoints[side], info.intersection));
         }

         return infer_map;
      }

      edges_map_t infer_t::inferGirih(const polygon_t& tile, int starSides, double starSkip)
      {
         edges_map_t infer_map;

         // We use the number of side of the star and how many side it
         // hops over from branch to branch (where 1 would mean drawing
         // a polygon) and deduce the inner angle of the star branches.
         // We support fractional side skipping.
         const double polygonInnerAngle = PI * (starSides - 2) / starSides;
         const double starBranchInnerAngle = (starSkip * polygonInnerAngle) - (starSkip - 1) * PI;
         const double requiredRotation = (PI - starBranchInnerAngle) / 2;

         // Get the index of a good transform for this tile.
         const placed_points_t* pmain = findPrimaryFeature(tile);
         if (!pmain)
            return infer_map;

         const std::vector<point_t>& mid_points = pmain->mids;

         std::vector<point_t> points = geometry::apply(pmain->trf, pmain->tile->points);

         const int side_count = length(mid_points);
         for (int side = 0; side < side_count; ++side)
         {
            edges_map_t branchMap = buildGirihBranch(side, requiredRotation, points, mid_points);
            if (branchMap.all().size() > 0)
               infer_map.merge(branchMap);
         }

         infer_map.apply_to_self(pmain->trf.invert());

         return infer_map;
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // Intersect inferring.

      int infer_t::getIntersectionRank(int side, bool isLeft, const std::vector<intersection_info>& infos)
      {
         for (int i = 0; i < length(infos); ++i)
            if (infos[i].otherSide == side && infos[i].otherIsLeft == isLeft)
               return i;
         return 10000;  // TODO: magic number!!!
      }

      std::vector<edges_length_info_t> infer_t::buildIntersectEdgesLengthInfos(
         const int side, const point_t& sideHalf, const bool isLeftHalf,
         const double requiredRotation,
         const std::vector<point_t>& points, const std::vector<point_t>& midPoints)
      {
         // First, get a list of intersections for this edge so that we can sort the
         // edge pairs by the fewest number of intersections.
         std::vector<intersection_info> inter_infos = buildIntersectionInfos(side, sideHalf, isLeftHalf,
                                                                             requiredRotation, points, midPoints);

         std::vector<edges_length_info_t> infos;

         const point_t& sideMidPoint = midPoints[side];

         const int side_count = length(points);
         for (int i_side = side + 1; i_side < side_count; ++i_side)
         {
            for (int i_halves = 0; i_halves < 2; ++i_halves)
            {
               const bool otherIsLeft = (i_halves == 0);
               const point_t& otherMidPoint = midPoints[i_side];
               point_t otherSide = buildGirihHalfBranch(i_side, otherIsLeft, requiredRotation, points, midPoints);
               std::vector<intersection_info> other_inter_infos = buildIntersectionInfos(i_side, otherSide, otherIsLeft,
                                                                                         requiredRotation, points, midPoints);

               point_t intersection = intersect(otherMidPoint, otherSide, sideMidPoint, sideHalf);
               if (intersection.is_invalid())
               {
                  // Lines are parallel, see if they actually point at each other.
                  if (utility::near_zero(otherMidPoint.distance_2_to_line(sideMidPoint, sideHalf)))
                  {
                     // Edge meets directly the other mid-points, so the distance is the middle in-between.
                     intersection = otherMidPoint.convex_sum(sideMidPoint, 0.5);
                  }
               }
               if (!intersection.is_invalid())
               {
                  if (polygon_t(points).is_inside(intersection))
                  {
                     const int inter_rank = getIntersectionRank(i_side, otherIsLeft, inter_infos);
                     const int other_rank = getIntersectionRank(side, isLeftHalf, other_inter_infos);
                     const double distance_2 = intersection.distance_2(sideMidPoint) + intersection.distance_2(otherMidPoint);
                     infos.emplace_back(edges_length_info_t(side, isLeftHalf, i_side, otherIsLeft, inter_rank + other_rank, distance_2, intersection));
                  }
               }
            }
         }

         return infos;
      }

      edges_map_t infer_t::inferIntersect(const polygon_t& tile, int starSides, double starSkip, int s)
      {
         edges_map_t infer_map;

         // We use the number of side of the star and how many side it
         // hops over from branch to branch (where 1 would mean drawing
         // a polygon) and deduce the inner angle of the star branches.
         // We support fractional side skipping.
         const double polygonInnerAngle = PI * (starSides - 2) / starSides;
         const double starBranchInnerAngle = (starSkip * polygonInnerAngle) - (starSkip - 1) * PI;
         const double requiredRotation = (PI - starBranchInnerAngle) / 2;

         // Get the index of a good transform for this tile.
         const placed_points_t* pmain = findPrimaryFeature(tile);
         if (!pmain)
            return infer_map;

         // Get the mid-points of each side of the tile.
         const std::vector<point_t>& mid_points = pmain->mids;

         // Get the corners of the tiles.
         std::vector<point_t> points = geometry::apply(pmain->trf, pmain->tile->points);

         // Accumulate all edge intersections and their length.
         std::vector<edges_length_info_t> infos;

         const int side_count = length(mid_points);
         for (int side = 0; side < side_count; ++side)
         {
            for (int i_halves = 0; i_halves < 2; ++i_halves)
            {
               const bool isLeftHalf = (i_halves == 0);
               point_t sideHalf = buildGirihHalfBranch(side, isLeftHalf, requiredRotation, points, mid_points);
               std::vector<edges_length_info_t> side_infos = buildIntersectEdgesLengthInfos(side, sideHalf, isLeftHalf,
                                                                                          requiredRotation, points, mid_points);
               if (side_infos.size() > 0)
               {
                  infos.insert(infos.end(), side_infos.begin(), side_infos.end());
               }
            }
         }

         // Sort edge-to-edge intersection by their total length.
         std::sort(infos.begin(), infos.end(), [](const edges_length_info_t& a, const edges_length_info_t& b) {
            return a.compareTo(b) < 0;
         });

         // Record the starting point of each edge. As we grow the edge,
         // when we want more than one intersection (s > 1), we will update
         // these starting points.
         std::vector<std::vector<point_t>> froms = std::vector<std::vector<point_t>>(side_count);
         std::vector<std::vector<int>> counts = std::vector<std::vector<int>>(side_count);
         for (int i = 0; i < side_count; ++i)
         {
            froms[i].push_back(mid_points[i]);
            froms[i].push_back(mid_points[i]);
            counts[i].push_back(0);
            counts[i].push_back(0);
         }

         // Build the edges map using the shortest edges first.
         for (int i = 0; i < length(infos); ++i)
         {
            const edges_length_info_t& info = infos[i];
            int side1 = info.side1;
            int isLeft1 = info.isLeft1 ? 0 : 1;
            int side2 = info.side2;
            int isLeft2 = info.isLeft2 ? 0 : 1;
            if (counts[side1][isLeft1] < s && counts[side2][isLeft2] < s)
            {
               const point_t& from1 = froms[side1][isLeft1];
               const point_t& from2 = froms[side2][isLeft2];
               if (!info.intersection.is_invalid())
               {
                  infer_map.insert(edge_t(from1, info.intersection));
                  infer_map.insert(edge_t(info.intersection, from2));
                  froms[side1][isLeft1] = froms[side2][isLeft2] = info.intersection;
               }
               else
               {
                  infer_map.insert(edge_t(from1, from2));
                  froms[side1][isLeft1] = from2;
                  froms[side2][isLeft2] = from1;
               }
               counts[side1][isLeft1] += 1;
               counts[side2][isLeft2] += 1;
            }
         }

         infer_map.apply_to_self(pmain->trf.invert());

         return infer_map;
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // Progressive intersect inferring.

      std::vector<intersection_info> infer_t::buildIntersectionInfos(
         const int side, point_t sideHalf, const bool isLeftHalf,
         const double requiredRotation,
         const std::vector<point_t>& points, const std::vector<point_t>& midPoints)
      {
         std::vector<intersection_info> infos;

         const point_t& sideMidPoint = midPoints[side];

         const int side_count = length(points);
         for (int i_side = 0; i_side < side_count; ++i_side)
         {
            if (i_side == side)
               continue;

            for (int i_halves = 0; i_halves < 2; ++i_halves)
            {
               const bool otherIsLeft = (i_halves == 0);
               const point_t& otherMidPoint = midPoints[i_side];

               point_t otherSide = buildGirihHalfBranch(i_side, otherIsLeft, requiredRotation, points, midPoints);
               point_t intersection = intersect(otherMidPoint, otherSide, sideMidPoint, sideHalf);
               if (intersection.is_invalid())
               {
                  if (utility::near_zero(otherMidPoint.distance_2_to_line(sideMidPoint, sideHalf))) {
                     // Edge meets directly the other mid-points, so the distance is the middle in-between.
                     intersection = otherMidPoint.convex_sum(sideMidPoint, 0.5);
                  }
               }
               if (!intersection.is_invalid())
               {
                  double distance_2 = intersection.distance_2(sideMidPoint);
                  infos.emplace_back(intersection_info(side, i_side, otherIsLeft, distance_2, intersection));
               }
            }
         }

         // Sort edge-to-edge intersection by their total length.
         std::sort(infos.begin(), infos.end(), [](const intersection_info& a, const intersection_info& b) {
            return a.compareTo(b) < 0;
         });

         return infos;
      }

      edges_map_t infer_t::inferIntersectProgressive(const polygon_t& tile, int starSides, double starSkip, int s)
      {
         edges_map_t infer_map;

         // We use the number of side of the star and how many side it
         // hops over from branch to branch (where 1 would mean drawing
         // a polygon) and deduce the inner angle of the star branches.
         // We support fractional side skipping.
         const double polygonInnerAngle = PI * (starSides - 2) / starSides;
         const double starBranchInnerAngle = (starSkip * polygonInnerAngle) - (starSkip - 1) * PI;
         const double requiredRotation = (PI - starBranchInnerAngle) / 2;

         // Get the index of a good transform for this tile.
         const placed_points_t* pmain = findPrimaryFeature(tile);
         if (!pmain)
            return infer_map;

         // Get the mid-points of each side of the tile.
         const std::vector<point_t>& mid_points = pmain->mids;

         // Get the corners of the tiles.
         std::vector<point_t> points = geometry::apply(pmain->trf, pmain->tile->points);

         const int side_count = length(mid_points);
         for (int side = 0; side < side_count; ++side)
         {
            for (int i_halves = 0; i_halves < 2; ++i_halves)
            {
               const bool isLeftHalf = (i_halves == 0);
               point_t sideHalf = buildGirihHalfBranch(side, isLeftHalf, requiredRotation, points, mid_points);
               std::vector<intersection_info> infos = buildIntersectionInfos(side, sideHalf, isLeftHalf,
                                                                             requiredRotation, points, mid_points);
               if (infos.size() > 0)
               {
                  // Record the starting point of the edge. As we grow the edge,
                  // when we want more than one intersection (s > 1), we will update
                  // this starting point.
                  point_t from = mid_points[side];

                  // Build the edges map using the shortest edges first.
                  int count = 0;
                  for (int i = 0; i < length(infos) && count < s; ++i)
                  {
                     const intersection_info& info = infos[i];
                     if (!info.intersection.is_invalid())
                     {
                        infer_map.insert(edge_t(from, info.intersection));
                        from = info.intersection;
                        count++;
                     }
                  }
               }
            }
         }

         infer_map.apply_to_self(pmain->trf.invert());

         return infer_map;
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // Hourglass inferring.

      edges_map_t infer_t::inferHourglass(const polygon_t& tile, double d, int s)
      {
         edges_map_t infer_map;

         // Get the index of a good transform for this tile.
         const placed_points_t* pmain = findPrimaryFeature(tile);
         if (!pmain)
            return infer_map;

         const std::vector<point_t>& mid_points = pmain->mids;

         int side_count = length(mid_points);

         // Fix the s value to be between [0, side_count / 2 - 1]
         // instead of [1, side_count / 2].
         int side_modulo;
         if ((side_count & 1) != 0)
         {
            side_modulo = side_count;
         }
         else
         {
            side_modulo = side_count / 2;
         }
         s = s % side_modulo;

         for (int side = 0; side < side_count; ++side)
         {
            double side_frac = (double) side / (double) side_count;
            double hour_d_pos = (side) % side_modulo != s ? d : 1.0;
            double hour_d_neg = (side + side_count - 1) % side_modulo != s ? d : 1.0;
            infer_map.merge(buildStarHalfBranch(hour_d_pos, 1, side_frac, 1.0, mid_points));
            infer_map.merge(buildStarHalfBranch(hour_d_neg, 1, side_frac, -1.0, mid_points));
         }

         infer_map.apply_to_self(pmain->trf.invert());

         return infer_map;
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // Rosette inferring.

      static const int TIP_POINT = 0;
      static const int QE_POINT = 1;
      static const int F_POINT = 2;

      std::vector<point_t> infer_t::buildRosetteBranchPoints(
         double q, int s, double r,
         double side_frac, double sign,
         const std::vector<point_t>& mid_points_orig,
         const std::vector<point_t>& corner_points_orig)
      {
         int side_count = length(mid_points_orig);
         double circle_frac = 1 / (double) side_count;

         point_t center = geometry::center(mid_points_orig);
         const std::vector<point_t> mid_points = geometry::translate(mid_points_orig, center.scale(-1));
         const std::vector<point_t> corner_points = geometry::translate(corner_points_orig, geometry::center(corner_points_orig).scale(-1));

         point_t tip = getArc(side_frac, mid_points);                        // The point to build from.
         point_t rtip = getArc(side_frac + sign * circle_frac, mid_points);  // The next point over.

         double q_clamp = std::min(std::max(q, -0.99), 0.99);
         int s_clamp = std::min(s, side_count / 2);

         // Consider an equilateral triangle formed by the origin,
         // up_outer and a vertical edge extending down from up_outer.
         // The center of the bottom edge of that triangle defines the
         // bisector of the angle leaving up_outer that we care about.
         point_t up_outer = getArc(side_frac + circle_frac, corner_points);
         point_t down_outer = getArc(side_frac, corner_points);
         if (sign < 0.0)
         {
            point_t temp = up_outer;
            up_outer = down_outer;
            down_outer = temp;
         }

         point_t bisector = down_outer.scale(0.5);
         bisector = bisector - up_outer;
         bisector = bisector.scale(10.0); // TODO: magic number
         bisector = bisector + up_outer;

         point_t e = intersect(up_outer, bisector, tip, rtip);
         if (e.is_invalid())
            e = up_outer.convex_sum(point_t::origin(), 0.5);
         point_t ad = intersect(up_outer, bisector, tip, point_t::origin());
         if (ad.is_invalid())
            ad = point_t::origin();
         point_t qe = q_clamp >= 0 ? e.convex_sum(up_outer, q) : e.convex_sum(ad, -q);

         point_t f = up_outer.scale(r);

         std::vector<point_t> points = std::vector<point_t>(3);
         points[TIP_POINT] = tip;
         points[QE_POINT] = qe;
         points[F_POINT] = f;

         return geometry::translate(points, center);
      }

      std::vector<point_t> infer_t::buildRosetteIntersections(
         double q, int s, double r,
         double side_frac, double sign,
         const std::vector<point_t>& mid_points,
         const std::vector<point_t>& corner_points,
         const std::vector<point_t>& points)
      {
         int side_count = length(mid_points);
         double circle_frac = 1. / side_count;

         std::vector<point_t> intersections;

         point_t qe_f = points[F_POINT] - points[QE_POINT];
         double other_sign = -1.0;
         {
            for (int is = 1; is <= s + 1 && length(intersections) < s; ++is)
            {
               std::vector<point_t> other_points = buildRosetteBranchPoints(
                  q, s, r, side_frac + sign * circle_frac * is, other_sign * sign, mid_points, corner_points);
               point_t other_qe_f = other_points[F_POINT] - other_points[QE_POINT];
               point_t meet_f = intersect(
                  points[QE_POINT], points[QE_POINT] + qe_f.scale(10.0),
                  other_points[QE_POINT], other_points[QE_POINT] + other_qe_f.scale(10.0));
               if (meet_f.is_invalid())
                  continue;
               intersections.emplace_back(meet_f);
            }
         }

         const point_t& qe = points[QE_POINT];
         std::sort(intersections.begin(), intersections.end(), [qe](const point_t& a, const point_t& b) {
            return (a - qe).mag_2() < (b - qe).mag_2();
         });

         return intersections;
      }


      edges_map_t infer_t::buildRosetteHalfBranch(
         double q, int s, double r,
         double side_frac, double sign,
         const std::vector<point_t>& mid_points,
         const std::vector<point_t>& corner_points)
      {
         std::vector<point_t> points = buildRosetteBranchPoints(q, s, r, side_frac, sign, mid_points, corner_points);
         std::vector<point_t> intersections = buildRosetteIntersections(q, s, r, side_frac, sign, mid_points, corner_points, points);

         edges_map_t from;

         for (int idx = 0; idx < length(points) - 1; ++idx)
            from.insert(edge_t(points[idx], points[idx + 1]));

         point_t prev = points.back();
         for (int is = 0; is < s && is < length(intersections); ++is)
         {
            from.insert(edge_t(prev, intersections[is]));
            prev = intersections[is];
         }

         return from;
      }

      edges_map_t infer_t::inferRosette(const polygon_t& tile, double q, int s, double r)
      {
         edges_map_t infer_map;

         // Get the index of a good transform for this tile.
         const placed_points_t* pmain = findPrimaryFeature(tile);
         if (!pmain)
            return infer_map;

         const std::vector<point_t>& mid_points = pmain->mids;
         const std::vector<point_t> corner_points = geometry::apply(pmain->trf, tile.points);

         int side_count = length(mid_points);
         for (int side = 0; side < side_count; ++side)
         {
            double side_frac = (double) side / (double) side_count;
            infer_map.merge(buildRosetteHalfBranch(q, s, r, side_frac,  1.0, mid_points, corner_points));
            infer_map.merge(buildRosetteHalfBranch(q, s, r, side_frac, -1.0, mid_points, corner_points));
         }

         infer_map.apply_to_self(pmain->trf.invert());

         return infer_map;
      }

      ////////////////////////////////////////////////////////////////////////////
      //
      // "Normal" magic inferring.

      edges_map_t infer_t::simple_infer(const polygon_t& tile)
      {
         edges_map_t infer_map;

         // Get the index of a good transform for this tile.
         const placed_points_t* pmain = findPrimaryFeature(tile);
         if (!pmain)
            return infer_map;

         const std::vector<point_t> fpts = geometry::apply(pmain->trf, pmain->tile->points);
         const polygon_t fpts_poly(fpts);

         std::vector<adjacency_info> adjs = getAdjacencies(*pmain);
         std::vector<contact_t> cons;
         buildContacts(*pmain, adjs, cons);

         // For every contact, if it hasn't found an extension,
         // Look at all other contacts for likely candidates.
         for (int idx = 0; idx < length(cons); ++idx)
         {
            contact_t& con = cons[idx];
            if (con.taken)
               continue;

            int jbest = -1;

            point_t bestisect;
            double bestdist = 0.0;
            int bestkind = NONE;

            for (int jdx = 0; jdx < length(cons); ++jdx)
            {
               point_t isect;

               if (jdx == idx)
                  continue;

               const contact_t& ocon = cons[jdx];

               if (ocon.taken)
                  continue;

               // Don't try on two contacts that involve the same vertex.
               if (geometry::near(con.position, ocon.position))
                  continue;

               double mydist = 0.0;
               int mykind = NONE;

               // First check if everybody's colinear.
               if (geometry::is_colinear(con.other, con.position, ocon.position) &&
                   geometry::is_colinear(con.position, ocon.position, ocon.other))
               {
                  // The two segments have to point at each other.
                  point_t d1 = con.position - con.other;
                  point_t d2 = ocon.position - ocon.other;
                  point_t dc = con.position - ocon.position;

                  // They point in the same direction.
                  if (d1.dot(d2) > 0.0)
                     continue;

                  // The first segment must point at the second point.
                  if (d1.dot(dc) > 0.0)
                     continue;
                  if (d2.dot(dc) < 0.0)
                     continue;

                  mykind = INSIDE_COLINEAR;
                  mydist = con.position.distance(ocon.position);
               }
               else
               {
                  isect = intersect_within(con.position, con.end, ocon.position, ocon.end);
                  if (isect.is_invalid())
                     continue;

                  // We don't want the case where the intersection
                  // lies too close to either vertex.  Note that
                  // I think these checks are subsumed by 
                  // getTrueIntersection.
                  if (geometry::near(isect, con.position))
                     continue;
                  if (geometry::near(isect, ocon.position))
                     continue;

                  double dist = con.position.distance(isect);
                  double odist = ocon.position.distance(isect);

                  bool inside = fpts_poly.is_inside(isect);

                  if (!utility::near(dist, odist))
                  {
                     if (inside)
                        mykind = INSIDE_UNEVEN;
                     else
                        mykind = OUTSIDE_UNEVEN;
                     mydist = std::abs(dist - odist);
                  }
                  else
                  {
                     if (inside)
                        mykind = INSIDE_EVEN;
                     else
                        mykind = OUTSIDE_EVEN;
                     mydist = dist;
                  }
               }

               if (lexCompareDistances(mykind, mydist, bestkind, bestdist) < 0)
               {
                  jbest = jdx;
                  bestkind = mykind;
                  bestdist = mydist;
                  bestisect = isect;
               }
            }

            if (jbest == -1)
               continue;

            contact_t& ocon = cons[jbest];
            con.taken = true;
            ocon.taken = true;

            if (bestkind == INSIDE_COLINEAR)
            {
               con.colinear = contact_t::COLINEAR_MASTER;
               ocon.colinear = contact_t::COLINEAR_SLAVE;
            }
            else
            {
               con.isect = bestisect;
               ocon.isect = bestisect;
            }

            con.isect_idx = jbest;
            ocon.isect_idx = idx;
         }

         // Using the stored intersections in the contacts, 
         // build a inferred edges map.

         for (int idx = 0; idx < length(cons); ++idx)
         {
            const contact_t& con = cons[idx];

            if (con.isect.is_invalid())
            {
               if (con.colinear == contact_t::COLINEAR_MASTER)
               {
                  infer_map.insert(edge_t(con.position, cons[con.isect_idx].position));
               }
            }
            else
            {
               infer_map.insert(edge_t(con.position, con.isect));
            }
         }

         // Try to link up unlinked edges.
         double minlen = fpts[0].distance(fpts[length(fpts) - 1]);
         for (int idx = 1; idx < length(fpts); ++idx)
            minlen = std::min(minlen, fpts[idx - 1].distance(fpts[idx]));

         for (int idx = 0; idx < length(cons); ++idx)
         {
            contact_t& con = cons[idx];
            if (con.isect_idx == -1)
            {
               for (int jdx = 0; jdx < length(cons); ++jdx)
               {
                  if (jdx == idx)
                     continue;

                  contact_t& ocon = cons[jdx];
                  if (ocon.isect_idx != -1)
                     continue;

                  // Two unmatched edges.  match them up.
                  point_t ex1 = con.position + con.position - con.other.normalize().scale(minlen*0.5);
                  point_t ex2 = ocon.position + ocon.position - ocon.other.normalize().scale(minlen*0.5);

                  infer_map.insert(edge_t(con.position, ex1));
                  infer_map.insert(edge_t(ex1, ex2));
                  infer_map.insert(edge_t(ex2, ocon.position));

                  con.isect_idx = jdx;
                  ocon.isect_idx = idx;
               }
            }
         }

         infer_map.apply_to_self(pmain->trf.invert());

         return infer_map;
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
