#include <dak/tiling/known_tilings_generator.h>
#include <dak/tiling/rosette.h>
#include <dak/tiling/star.h>
#include <dak/tiling/irregular_figure.h>

#include <dak/geometry/utility.h>

#include <algorithm>

namespace dak
{
   namespace tiling
   {
      known_tilings_generator_t::known_tilings_generator_t(const std::wstring& folder, std::vector<std::wstring>& errors)
      : tilings(folder, errors)
      {
         iter = tilings.tilings.end();
      }

      known_tilings_generator_t& known_tilings_generator_t::next()
      {
         if (iter == tilings.tilings.end() || iter == std::prev(tilings.tilings.end()))
            iter = tilings.tilings.begin();
         else
            ++iter;
         generate_mosaic();
         return *this;
      }

      known_tilings_generator_t& known_tilings_generator_t::previous()
      {
         if (iter == tilings.tilings.begin())
            iter = std::prev(tilings.tilings.end());
         else
            --iter;
         generate_mosaic();
         return *this;
      }

      known_tilings_generator_t& known_tilings_generator_t::set_index(int index)
      {
         if (index < 0 || index >= tilings.tilings.size())
            return *this;

         iter = tilings.tilings.begin() + index;
         generate_mosaic();
         return *this;
      }

      const std::wstring known_tilings_generator_t::current_name() const
      {
         if (iter == tilings.tilings.end())
            return L"";
         else
            return iter->name;
      }

      // Create a mosaic with the tiling.
      void known_tilings_generator_t::generate_mosaic()
      {
         const auto& tiling = *iter;
         mo = std::make_shared<mosaic_t>(tiling);

         // Fill all regular tiles with stars.
         for (const auto& placed : mo->tiling.tiles)
         {
            const auto& tile = placed.first;
            if (tile.is_regular())
            {
               mo->tile_figures[tile] = std::make_shared<star_t>(int(tile.points.size()), tile.points.size() / 3., 3);
            }
         }

         // Fill all irregular tiles with inferred girih.
         for (const auto& placed : mo->tiling.tiles)
         {
            const auto& tile = placed.first;
            if (!tile.is_regular())
            {
               mo->tile_figures[tile] = std::make_shared<irregular_figure_t>(mo, tile, infer_mode_t::girih, std::max(1.3, tile.points.size() / 3.));
            }
         }
      }

      map known_tilings_generator_t::generate_tiling_map(const rect& region) const
      {
         return mo->construct(region);
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
