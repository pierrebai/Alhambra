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

      tiling_t read_tiling(std::wistream& file);

      void write_tiling(const tiling_t& t, std::wostream& file);
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
