#include <dak/tiling/known_tilings.h>
#include <dak/tiling/tiling_io.h>
#include <dak/tiling/mosaic.h>
#include <dak/tiling/rosette.h>
#include <dak/tiling/star.h>
#include <dak/tiling/irregular_figure.h>

#include <dak/geometry/utility.h>

#include <dak/utility/text.h>

#include <filesystem>
#include <fstream>

namespace dak
{
   namespace tiling
   {
      known_tilings_t read_tilings(const std::wstring& folder, std::vector<std::wstring>& errors)
      {
         known_tilings_t tilings;

         try
         {
            std::experimental::filesystem::directory_iterator dir(folder);
            for (const auto& entry : dir)
            {
               try
               {
                  std::wifstream file(entry.path());
                  tilings.emplace_back(read_tiling(file));
               }
               catch (const std::exception& ex)
               {
                  errors.emplace_back(utility::convert(ex.what()));
               }
            }
         }
         catch (const std::exception& ex)
         {
            errors.emplace_back(utility::convert(ex.what()));
         }

         return tilings;
      }

      std::shared_ptr<mosaic_t> generate_mosaic(const tiling_t& tiling)
      {
         auto mo = std::make_shared<mosaic_t>(tiling);

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

         return mo;
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
