#pragma once

#ifndef DAK_TILING_STYLE_KNOWN_MOSAICS_H
#define DAK_TILING_STYLE_KNOWN_MOSAICS_H

#include <dak/tiling/tiling.h>

#include <dak/tiling_style/style.h>

#include <string>
#include <vector>

namespace dak
{
   namespace tiling
   {
      class tiling;
      using known_tilings = std::vector<tiling>;
   }

   namespace tiling_style
   {
      using tiling::known_tilings;

      ////////////////////////////////////////////////////////////////////////////
      // 
      // Reads all mosaic files in a given folder.

      class known_mosaics
      {
      public:
         typedef std::vector<std::shared_ptr<layer>> layered_mosaic;

         std::vector<layered_mosaic> mosaics;

         known_mosaics(const std::wstring& folder, const known_tilings& knowns, std::vector<std::wstring>& errors) { read_mosaics(folder, knowns, errors); }

         void read_mosaics(const std::wstring& folder, const known_tilings& knowns, std::vector<std::wstring>& errors);
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
