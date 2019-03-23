#include <dak/tiling_style/known_mosaics.h>
#include <dak/tiling_style/style_io.h>

#include <dak/geometry/utility.h>

#include <filesystem>
#include <fstream>

namespace dak
{
   namespace tiling_style
   {
      void known_mosaics::read_mosaics(const std::wstring& folder, const known_tilings& knowns, std::vector<std::wstring>& errors)
      {
         try
         {
            std::experimental::filesystem::directory_iterator dir(folder);
            for (const auto& entry : dir)
            {
               try
               {
                  std::wifstream file(entry.path());
                  mosaics.emplace_back(read_layered_mosaic(file, knowns));
               }
               catch (const std::exception& ex)
               {
                  errors.emplace_back(geometry::convert(ex.what()));
               }
            }
         }
         catch (const std::exception& ex)
         {
            errors.emplace_back(geometry::convert(ex.what()));
         }
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
