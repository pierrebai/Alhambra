#include <dak/tiling/tiling_selection.h>

#include <dak/geometry/utility.h>

namespace dak
{
   namespace tiling
   {
      namespace tiling_selection
      {
         selection find_selection(std::vector<std::shared_ptr<placed_tile>>& tiles, const point& wpt, double selection_distance)
         {
            return find_selection(tiles, wpt, selection_distance, selection(), selection_type::all);
         }

         selection find_selection(std::vector<std::shared_ptr<placed_tile>>& tiles, const point& wpt, double sel_dist, const selection& excluded_sel)
         {
            return find_selection(tiles, wpt, sel_dist, excluded_sel, selection_type::all);
         }

         selection find_selection(std::vector<std::shared_ptr<placed_tile>>& tiles, const point& wpt, double sel_dist, const selection& excluded_sel, selection_type sel_types)
         {
            const double sel_dist_2 = sel_dist * sel_dist;
            const std::shared_ptr<placed_tile> other_than = get_placed_tile(excluded_sel);
            for (auto iter = tiles.rbegin(); iter != tiles.rend(); ++iter)
            {
               auto& placed = *iter;

               if (placed->tile.is_invalid())
                  continue;

               if (other_than == placed)
                  continue;

               selection new_sel;

               const point lpt = wpt.apply(placed->trf.invert());
               if ((sel_types & selection_type::tile) == selection_type::tile)
                  if (placed->tile.is_inside(lpt))
                     new_sel.add(tile_selection{placed});

               if ((sel_types & selection_type::point) == selection_type::point)
                  if (geometry::near(lpt, placed->tile.center(), sel_dist_2))
                  new_sel.add(point_selection(placed));

               size_t prev_i = placed->tile.points.size() - 1;
               for (size_t i = 0; i < placed->tile.points.size(); ++i)
               {
                  const point& pt = placed->tile.points[i];

                  if ((sel_types & selection_type::point) == selection_type::point)
                     if (geometry::near(lpt, pt, sel_dist_2))
                        new_sel.add(point_selection(placed, i));

                  const point& prev_pt = placed->tile.points[prev_i];

                  if ((sel_types & selection_type::point) == selection_type::point)
                        if (geometry::near(lpt, prev_pt.convex_sum(pt, 0.5), sel_dist_2))
                           new_sel.add(point_selection(placed, prev_i, i));

                  if ((sel_types & selection_type::edge) == selection_type::edge)
                  {
                     edge_selection new_edge_sel = { placed, prev_i, i };
                     if (utility::near_less(lpt.distance_2_to_line(prev_pt, pt), sel_dist_2))
                        new_sel.add(new_edge_sel);
                  }

                  prev_i = i;
               }

               if (new_sel.has_selection())
                  return new_sel;
            }

            return selection();
         }

         tile_selection get_placed_tile(const selection& sel)
         {
            for (const std::any& data : sel.data)
            {
               if (const tile_selection* placed = std::any_cast<tile_selection>(&data))
               {
                  return *placed;
               }
               if (const edge_selection* edge = std::any_cast<edge_selection>(&data))
               {
                  return *edge;
               }
               if (const point_selection* pt = std::any_cast<point_selection>(&data))
               {
                  return *pt;
               }
            }
            return tile_selection();
         }

         edge_selection get_edge(const selection& sel)
         {
            for (const std::any& data : sel.data)
            {
               if (const edge_selection* edge = std::any_cast<edge_selection>(&data))
               {
                  return *edge;
               }
            }
            return edge_selection();
         }

         point_selection get_point(const selection& sel)
         {
            for (const std::any& data : sel.data)
            {
               if (const point_selection* pt = std::any_cast<point_selection>(&data))
               {
                  return *pt;
               }
            }
            return point_selection();
         }
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
