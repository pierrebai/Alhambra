#include <dak/tiling_style/known_mosaics_generator.h>
#include <dak/tiling_style/mosaic_io.h>

#include <dak/tiling/known_tilings.h>

#include <dak/geometry/polygon.h>
#include <dak/geometry/utility.h>

#include <dak/utility/text.h>

#include <fstream>

namespace dak
{
   namespace tiling_style
   {
      known_mosaics_generator_t::known_mosaics_generator_t()
         : iter(filenames.end())
      {
      }

      void known_mosaics_generator_t::add_folder(const std::wstring& folder)
      {
         try
         {
            for (const auto entry : std::filesystem::directory_iterator(folder))
               filenames.emplace_back(entry.path());
            iter = filenames.end();
         }
         catch (const std::exception&)
         {
            // Ignore: folder does not exists.
         }
      }

      known_mosaics_generator_t& known_mosaics_generator_t::next()
      {
         if (iter == filenames.end() || iter == std::prev(filenames.end()))
            iter = filenames.begin();
         else
            ++iter;
         return *this;
      }

      known_mosaics_generator_t& known_mosaics_generator_t::previous()
      {
         if (iter == filenames.begin())
            iter = std::prev(filenames.end());
         else
            --iter;
         return *this;
      }

      const std::wstring known_mosaics_generator_t::current_name() const
      {
         if (iter == filenames.end())
            return L"";
         else
            return iter->filename();
      }

      known_mosaics_generator_t::layered_mosaic_t known_mosaics_generator_t::generate_current(const tiling::known_tilings_t& known_tilings, std::vector<std::wstring>& errors) const
      {
         static layered_mosaic_t empty;
         if (iter == filenames.end())
            return empty;

         try
         {
            std::wifstream file(*iter);
            return read_layered_mosaic(file, known_tilings);
         }
         catch (const std::exception& ex)
         {
            errors.emplace_back(utility::convert(ex.what()));
            return empty;
         }
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
