#include <dak/tiling/tiling_selection.h>

#include <dak/geometry/utility.h>

namespace dak
{
   namespace tiling
   {
      namespace tiling_selection
      {
         selection_t find_selection(std::vector<std::shared_ptr<placed_tile_t>>& tiles, const point_t& wpt, double selection_distance)
         {
            return find_selection(tiles, wpt, selection_distance, selection_t(), selection_type_t::all);
         }

         selection_t find_selection(std::vector<std::shared_ptr<placed_tile_t>>& tiles, const point_t& wpt, double sel_dist, const selection_t& excluded_sel)
         {
            return find_selection(tiles, wpt, sel_dist, excluded_sel, selection_type_t::all);
         }

         selection_t find_selection(std::vector<std::shared_ptr<placed_tile_t>>& tiles, const point_t& wpt, double sel_dist, const selection_t& excluded_sel, selection_type_t sel_types)
         {
            const bool is_single_type = is_selection_single_type(sel_types);
            if (is_single_type)
               sel_dist *= 4;

            const double sel_dist_2 = sel_dist * sel_dist;
            const std::shared_ptr<placed_tile_t> other_than = get_placed_tile(excluded_sel);
            for (auto iter = tiles.rbegin(); iter != tiles.rend(); ++iter)
            {
               auto& placed = *iter;

               if (placed->tile.is_invalid())
                  continue;

               if (other_than == placed)
                  continue;

               selection_t new_sel;

               const auto placed_tile = placed->tile.apply(placed->trf);

               if ((sel_types & selection_type_t::tile) == selection_type_t::tile)
                  if (placed_tile.is_inside(wpt))
                     new_sel.add(tile_selection_t{placed});

               // When selecting anything, don't select points or edges if the tile is too small.
               // Here we define too small as 4 times the area, which is 16 when squared.
               const bool small_tile = (placed_tile.area() < sel_dist_2 * 16);
               if (small_tile && !is_single_type)
                  goto check_sel;

               if ((sel_types & selection_type_t::point) == selection_type_t::point)
                  if (geometry::near(wpt, placed_tile.center(), sel_dist_2))
                     new_sel.add(point_selection_t(placed));

               if (small_tile && !is_single_type)
                  goto check_sel;

               {
                  size_t prev_i = placed_tile.points.size() - 1;
                  for (size_t i = 0; i < placed_tile.points.size(); ++i)
                  {
                     const point_t& pt = placed_tile.points[i];

                     const point_t& prev_pt = placed_tile.points[prev_i];

                     // Don't allow selecting end-points of the edge when the edge is too short.
                     // Here we define too short as 4 times the selection distance, which is 16 when squared.
                     if (is_single_type || pt.distance_2(prev_pt) > sel_dist_2 * 16)
                        if ((sel_types & selection_type_t::point) == selection_type_t::point)
                           if (geometry::near(wpt, pt, sel_dist_2))
                              new_sel.add(point_selection_t(placed, i));

                     // Don't allow selecting the middle of the edge when the edge is too short.
                     // Here we define too short as 6 times the selection distance, which is 36 when squared.
                     if (is_single_type || pt.distance_2(prev_pt) > sel_dist_2 * 36)
                        if ((sel_types & selection_type_t::point) == selection_type_t::point)
                           if (geometry::near(wpt, prev_pt.convex_sum(pt, 0.5), sel_dist_2))
                              new_sel.add(point_selection_t(placed, prev_i, i));

                     if ((sel_types & selection_type_t::edge) == selection_type_t::edge)
                        if (utility::near_less(wpt.distance_2_to_line(prev_pt, pt), sel_dist_2))
                           new_sel.add(edge_selection_t{ placed, prev_i, i });

                     prev_i = i;
                  }
               }

            check_sel:
               if (new_sel.has_selection())
                  return new_sel;
            }

            return selection_t();
         }

         tile_selection_t get_placed_tile(const selection_t& sel)
         {
            for (const std::any& data : sel.data)
            {
               if (const tile_selection_t* placed = std::any_cast<tile_selection_t>(&data))
               {
                  return *placed;
               }
               if (const edge_selection_t* edge = std::any_cast<edge_selection_t>(&data))
               {
                  return *edge;
               }
               if (const point_selection_t* pt = std::any_cast<point_selection_t>(&data))
               {
                  return *pt;
               }
            }
            return tile_selection_t();
         }

         edge_selection_t get_edge(const selection_t& sel)
         {
            for (const std::any& data : sel.data)
            {
               if (const edge_selection_t* edge = std::any_cast<edge_selection_t>(&data))
               {
                  return *edge;
               }
            }
            return edge_selection_t();
         }

         point_selection_t get_point(const selection_t& sel)
         {
            for (const std::any& data : sel.data)
            {
               if (const point_selection_t* pt = std::any_cast<point_selection_t>(&data))
               {
                  return *pt;
               }
            }
            return point_selection_t();
         }
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
