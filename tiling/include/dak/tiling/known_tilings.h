#pragma once

#ifndef DAK_TILING_KNOWN_TILINGS_H
#define DAK_TILING_KNOWN_TILINGS_H

#include <memory>
#include <vector>

namespace dak
{
   namespace tiling
   {
      class mosaic;
      class tiling;

      ////////////////////////////////////////////////////////////////////////////
      // 
      // Reads all tiling files in a given folder.

      using known_tilings = std::vector<tiling>;

      known_tilings read_tilings(const std::wstring& folder, std::vector<std::wstring>& errors);
      std::shared_ptr<mosaic> generate_mosaic(const tiling& tiling);
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
