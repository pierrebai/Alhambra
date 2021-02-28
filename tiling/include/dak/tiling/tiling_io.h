#pragma once

#ifndef DAK_TILING_TILING_IO_H
#define DAK_TILING_TILING_IO_H

#include <dak/tiling/tiling.h>

#include <iostream>

namespace dak
{
   namespace tiling
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // Functions for reading and writing tilings from and to I/O streams.

      std::shared_ptr<tiling_t> read_tiling(std::wistream& file);

      void write_tiling(const std::shared_ptr<const tiling_t>& tiling, std::wostream& file);
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
