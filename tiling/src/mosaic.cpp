#include <dak/tiling/mosaic.h>

#include <dak/geometry/utility.h>
#include <dak/geometry/transform.h>

namespace dak
{
   namespace tiling
   {
      using geometry::map;
      using geometry::transform;

      mosaic::mosaic(const mosaic& other)
      : tiling(other.tiling)
      {
         for (const auto& tile_fig : other.tile_figures)
         {
            tile_figures[tile_fig.first] = tile_fig.second->clone();
         }
      }

      mosaic& mosaic::operator=(const mosaic& other)
      {
         mosaic copy(other);
         swap(copy);
         return *this;
      }

      void mosaic::swap(mosaic& other) noexcept
      {
         tiling.swap(other.tiling);
         tile_figures.swap(other.tile_figures);
      }

      bool mosaic::operator==(const mosaic& other) const
      {
         return tiling == other.tiling && same_figures(other);
      }

      bool mosaic::same_figures(const mosaic& other) const
      {
         if (tile_figures.size() != other.tile_figures.size())
            return false;

         for (const auto& tile_fig : tile_figures)
         {
            const auto other_tile_fig = other.tile_figures.find(tile_fig.first);
            if (other_tile_fig == other.tile_figures.end())
               return false;
            if (*(tile_fig.second) != *(other_tile_fig->second))
               return false;
         }

         return true;
      }

      map mosaic::construct(const rect& region) const
      {
         map final_map;
         geometry::fill(region, tiling.t1, tiling.t2, [&tiling=tiling, &tile_figures=tile_figures,&final_map=final_map](int t1, int t2)
         {
            const auto receive_trf = transform::translate(tiling.t1.scale(t1) + tiling.t2.scale(t2));
            for (const auto& tile_placements : tiling.tiles)
            {
               const auto iter = tile_figures.find(tile_placements.first);
               if (iter == tile_figures.end())
                  continue;

               const geometry::map& map = iter->second->get_map();
               for (const auto& trf : tile_placements.second)
               {
                  const transform total_trf = receive_trf.compose(trf);
                  const geometry::map placed = map.apply(total_trf);
                  final_map.merge_non_overlapping(placed);
                  //final_map.merge(placed);
               }
            }
         });
         return final_map;
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
