#include <dak/tiling/mosaic.h>

#include <dak/geometry/point_utility.h>
#include <dak/geometry/transform.h>

namespace dak
{
   namespace tiling
   {
      using geometry::map;
      using geometry::transform;

      map mosaic::construct(const rect& region) const
      {
         map final_map;
         geometry::fill(region, tiling.t1, tiling.t2, [&tiling=tiling, &tile_figures=tile_figures,&final_map=final_map](int t1, int t2)
         {
            const auto receive_trf = transform::translate(tiling.t1.scale(t1) + tiling.t2.scale(t2));
            for (const auto& placed_tile : tiling.tiles)
            {
               const auto iter = tile_figures.find(placed_tile.first);
               if (iter == tile_figures.end())
                  continue;

               const geometry::map& map = iter->second->get_map();
               for (const auto& trf : placed_tile.second)
               {
                  const transform total_trf = receive_trf.compose(trf);
                  const geometry::map placed = map.apply(total_trf);
                  final_map.merge(placed);
               }
            }
         });
         return final_map;
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
