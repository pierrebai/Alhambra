#include <dak/tiling/mosaic.h>

#include <dak/geometry/utility.h>
#include <dak/geometry/transform.h>

namespace dak
{
   namespace tiling
   {
      using geometry::edges_map_t;
      using geometry::transform_t;

      mosaic_t::mosaic_t(const mosaic_t& other)
      : tiling(other.tiling)
      {
         for (const auto& tile_fig : other.tile_figures)
         {
            tile_figures[tile_fig.first] = tile_fig.second->clone();
         }
      }

      mosaic_t& mosaic_t::operator=(const mosaic_t& other)
      {
         mosaic_t copy(other);
         swap(copy);
         return *this;
      }

      void mosaic_t::swap(mosaic_t& other) noexcept
      {
         tiling.swap(other.tiling);
         tile_figures.swap(other.tile_figures);
      }

      bool mosaic_t::operator==(const mosaic_t& other) const
      {
         return tiling == other.tiling && same_figures(other);
      }

      bool mosaic_t::same_figures(const mosaic_t& other) const
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

      size_t mosaic_t::count_tiling_edges() const
      {
         size_t count = 0;

         for (const auto& tile_placements : tiling->tiles)
         {
            const auto iter = tile_figures.find(tile_placements.first);
            if (iter == tile_figures.end())
               continue;

            const geometry::edges_map_t& edges_map_t = iter->second->get_map();
            count += edges_map_t.all().size() * tile_placements.second.size();
         }

         return count;
      }

      edges_map_t mosaic_t::construct(const rectangle_t& region) const
      {
         edges_map_t final_map;
         final_map.reserve(tiling->count_fill_copies(region) * count_tiling_edges());
         final_map.begin_merge_non_overlapping();
         tiling->fill(region, [&tile_figures=tile_figures,&final_map=final_map](const tiling_t& tiling, const transform_t& receive_trf)
         {
            for (const auto& tile_placements : tiling.tiles)
            {
               const auto iter = tile_figures.find(tile_placements.first);
               if (iter == tile_figures.end())
                  continue;

               const geometry::edges_map_t& edges_map_t = iter->second->get_map();
               for (const auto& trf : tile_placements.second)
               {
                  const transform_t total_trf = receive_trf.compose(trf);
                  const geometry::edges_map_t placed = edges_map_t.apply(total_trf);
                  final_map.merge_non_overlapping(placed);
                  //final_map.merge(placed);
               }
            }
         });
         final_map.end_merge_non_overlapping();
         return final_map;
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
