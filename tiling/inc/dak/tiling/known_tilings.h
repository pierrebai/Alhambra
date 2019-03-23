#pragma once

#ifndef DAK_TILING_KNOWN_TILINGS_H
#define DAK_TILING_KNOWN_TILINGS_H

#include <dak/tiling/tiling.h>

namespace dak
{
   namespace tiling
   {
      ////////////////////////////////////////////////////////////////////////////
      // 
      // Reads all tiling files in a given folder.

      class known_tilings
      {
      public:
         std::vector<tiling> tilings;

         known_tilings(const std::wstring& folder, std::vector<std::wstring>& errors) { read_tilings(folder, errors); }

         void read_tilings(const std::wstring& folder, std::vector<std::wstring>& errors);
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
