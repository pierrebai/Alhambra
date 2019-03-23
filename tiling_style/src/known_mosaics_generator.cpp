#include <dak/tiling_style/known_mosaics_generator.h>
#include <dak/tiling_style/style_io.h>

#include <dak/tiling/known_tilings.h>

#include <dak/geometry/polygon.h>
#include <dak/geometry/utility.h>

#include <fstream>

namespace dak
{
   namespace tiling_style
   {
      known_mosaics_generator::known_mosaics_generator(const std::wstring& folder, const tiling::known_tilings& knowns)
      : iter(filenames.end()), known_tilings(knowns)
      {
         for (const auto entry : std::experimental::filesystem::directory_iterator(folder))
            filenames.emplace_back(entry.path());
         iter = filenames.end();
      }

      known_mosaics_generator& known_mosaics_generator::next()
      {
         if (iter == filenames.end() || iter == std::prev(filenames.end()))
            iter = filenames.begin();
         else
            ++iter;
         return *this;
      }

      known_mosaics_generator& known_mosaics_generator::previous()
      {
         if (iter == filenames.begin())
            iter = std::prev(filenames.end());
         else
            --iter;
         return *this;
      }

      const std::wstring known_mosaics_generator::current_name() const
      {
         if (iter == filenames.end())
            return L"";
         else
            return iter->filename();
      }

      known_mosaics_generator::layered_mosaic known_mosaics_generator::generate_current(std::vector<std::wstring>& errors) const
      {
         static layered_mosaic empty;
         if (iter == filenames.end())
            return empty;

         try
         {
            std::wifstream file(*iter);
            return read_layered_mosaic(file, known_tilings);
         }
         catch (const std::exception& ex)
         {
            errors.emplace_back(geometry::convert(ex.what()));
            return empty;
         }
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
