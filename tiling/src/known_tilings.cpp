#include <dak/tiling/known_tilings.h>
#include <dak/tiling/tiling_io.h>

#include <dak/geometry/utility.h>

#include <filesystem>
#include <fstream>

namespace dak
{
   namespace tiling
   {
      void known_tilings::read_tilings(const std::wstring& folder, std::vector<std::wstring>& errors)
      {
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
