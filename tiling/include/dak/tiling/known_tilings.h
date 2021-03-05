#pragma once

#ifndef DAK_TILING_KNOWN_TILINGS_H
#define DAK_TILING_KNOWN_TILINGS_H

#include <map>
#include <memory>
#include <vector>
#include <string>
#include <filesystem>

namespace dak
{
   namespace tiling
   {
      class mosaic_t;
      class tiling_t;

      ////////////////////////////////////////////////////////////////////////////
      // 
      // Reads all tiling files in a given folder.

      using known_tilings_t = std::map<std::wstring, std::shared_ptr<tiling_t>>;

      known_tilings_t read_tilings(const std::wstring& folder, std::vector<std::wstring>& errors);

      std::wstring add_tiling(known_tilings_t& known_tilings, const std::shared_ptr<tiling_t>& tiling, const std::filesystem::path& path);
      std::shared_ptr<tiling_t> find_tiling(const known_tilings_t& known_tilings, const std::wstring& name);

      std::shared_ptr<mosaic_t> generate_mosaic(const std::shared_ptr<const tiling_t>& tiling);
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
