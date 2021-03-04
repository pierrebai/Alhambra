#pragma once

#ifndef DAK_TILING_TILING_STYLE_IO_H
#define DAK_TILING_TILING_STYLE_IO_H


#include <dak/tiling/known_tilings.h>
#include <dak/tiling_style/styled_mosaic.h>

#include <vector>
#include <iostream>

namespace dak
{
   namespace tiling_style
   {
      using tiling::known_tilings_t;

      ////////////////////////////////////////////////////////////////////////////
      //
      // Functions for reading and writing layers of styles from and to I/O streams.

      std::vector<std::shared_ptr<styled_mosaic_t>> read_layered_mosaic(std::wistream& file, const known_tilings_t& knowns);

      void write_layered_mosaic(std::wostream& file, const std::vector<std::shared_ptr<styled_mosaic_t>>& layers);
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
