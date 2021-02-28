#pragma once

#ifndef DAK_TILING_STYLE_KNOWN_MOSAICS_GENERATOR_H
#define DAK_TILING_STYLE_KNOWN_MOSAICS_GENERATOR_H

#include <dak/tiling_style/style.h>

#include <dak/tiling/known_tilings.h>

#include <filesystem>
#include <string>
#include <vector>

namespace dak
{
   namespace geometry
   {
      class rectangle_t;
   }

   namespace tiling_style
   {
      using geometry::rectangle_t;
      class styled_mosaic_t;
      using tiling::known_tilings_t;

      ////////////////////////////////////////////////////////////////////////////
      // 
      // Reads all layered mosaic files in a given folder and return them on-demand.

      class known_mosaics_generator_t
      {
      public:
         typedef std::vector<std::shared_ptr<styled_mosaic_t>> layered_mosaic_t;

         known_mosaics_generator_t();

         void add_folder(const std::wstring& folder);

         known_mosaics_generator_t& next();

         known_mosaics_generator_t& previous();

         const std::wstring current_name() const;

         layered_mosaic_t generate_current(const tiling::known_tilings_t& known_tilings, std::vector<std::wstring>& errors) const;

      private:
         typedef std::vector< std::filesystem::path> filename_container;
         filename_container filenames;
         filename_container::iterator iter;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
