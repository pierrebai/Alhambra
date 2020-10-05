#pragma once

#ifndef DAK_TILING_KNOWN_TILINGS_H
#define DAK_TILING_KNOWN_TILINGS_H

#include <memory>
#include <vector>

namespace dak
{
   namespace tiling
   {
      class mosaic_t;
      class tiling_t;

      ////////////////////////////////////////////////////////////////////////////
      // 
      // Reads all tiling files in a given folder.

      using known_tilings_t = std::vector<tiling_t>;

      known_tilings_t read_tilings(const std::wstring& folder, std::vector<std::wstring>& errors);
      std::shared_ptr<mosaic_t> generate_mosaic(const tiling_t& tiling);
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
