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
      class tiling_t;
      using known_tilings_t = std::vector<tiling_t>;
   }

   namespace tiling_style
   {
      using tiling::known_tilings_t;

      ////////////////////////////////////////////////////////////////////////////
      // 
      // Reads all mosaic files in a given folder.

      class known_mosaics_t
      {
      public:
         typedef std::vector<std::shared_ptr<layer_t>> layered_mosaic_t;

         std::vector<layered_mosaic_t> mosaics;

         known_mosaics_t(const std::wstring& folder, const known_tilings_t& knowns, std::vector<std::wstring>& errors) { read_mosaics(folder, knowns, errors); }

         void read_mosaics(const std::wstring& folder, const known_tilings_t& knowns, std::vector<std::wstring>& errors);
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
