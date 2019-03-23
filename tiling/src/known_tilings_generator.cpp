#include <dak/tiling/known_tilings_generator.h>
#include <dak/tiling/rosette.h>
#include <dak/tiling/star.h>
#include <dak/tiling/irregular_figure.h>

#include <dak/geometry/point_utility.h>

namespace dak
{
   namespace tiling
   {
      known_tilings_generator::known_tilings_generator(const std::wstring& folder, std::vector<std::wstring>& errors)
      : tilings(folder, errors)
      {
         iter = tilings.tilings.end();
      }

      known_tilings_generator& known_tilings_generator::next()
      {
         if (iter == tilings.tilings.end() || iter == std::prev(tilings.tilings.end()))
            iter = tilings.tilings.begin();
         else
            ++iter;
         generate_mosaic();
         return *this;
      }

      known_tilings_generator& known_tilings_generator::previous()
      {
         if (iter == tilings.tilings.begin())
            iter = std::prev(tilings.tilings.end());
         else
            --iter;
         generate_mosaic();
         return *this;
      }

      known_tilings_generator& known_tilings_generator::set_index(int index)
      {
         if (index < 0 || index >= tilings.tilings.size())
            return *this;

         iter = tilings.tilings.begin() + index;
         generate_mosaic();
         return *this;
      }

      const std::wstring known_tilings_generator::current_name() const
      {
         if (iter == tilings.tilings.end())
            return L"";
         else
            return iter->name;
      }

      // Create a mosaic with the tiling.
      void known_tilings_generator::generate_mosaic()
      {
         const auto& tiling = *iter;
         mo = std::make_shared<mosaic>(tiling);

         // Fill all regular tiles with stars.
         for (const auto& placed : mo->tiling.tiles)
         {
            const auto& tile = placed.first;
            if (tile.is_regular())
            {
               mo->tile_figures[tile] = std::make_shared<star>(int(tile.points.size()), tile.points.size() / 3., 3);
            }
         }

         // Fill all irregular tiles with inferred girih.
         for (const auto& placed : mo->tiling.tiles)
         {
            const auto& tile = placed.first;
            if (!tile.is_regular())
            {
               mo->tile_figures[tile] = std::make_shared<irregular_figure>(mo, tile, infer_mode::girih);
            }
         }
      }

      map known_tilings_generator::generate_tiling_map(const rect& region) const
      {
         return mo->construct(region);
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
