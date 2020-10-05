#pragma once

#ifndef DAK_TILING_STYLE_KNOWN_MOSAICS_GENERATOR_H
#define DAK_TILING_STYLE_KNOWN_MOSAICS_GENERATOR_H

#include <dak/tiling_style/style.h>

#include <filesystem>
#include <string>
#include <vector>

namespace dak
{
   namespace geometry
   {
      class rect;
   }

   namespace tiling
   {
      class tiling;
      using known_tilings = std::vector<tiling>;
   }

   namespace tiling_style
   {
      using geometry::rect;

      ////////////////////////////////////////////////////////////////////////////
      // 
      // Reads all layered mosaic files in a given folder and return them on-demand.

      class known_mosaics_generator
      {
      public:
         typedef std::vector<std::shared_ptr<layer>> layered_mosaic;

         known_mosaics_generator(const std::wstring& folder);

         known_mosaics_generator& next();

         known_mosaics_generator& previous();

         const std::wstring current_name() const;

         layered_mosaic generate_current(const tiling::known_tilings& known_tilings, std::vector<std::wstring>& errors) const;

      private:
         typedef std::vector< std::experimental::filesystem::path> filename_container;
         filename_container filenames;
         filename_container::iterator iter;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
