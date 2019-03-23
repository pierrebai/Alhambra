#pragma once

#ifndef DAK_TILING_KNOWN_TILINGS_GENERATOR_H
#define DAK_TILING_KNOWN_TILINGS_GENERATOR_H

#include <dak/tiling/known_tilings.h>
#include <dak/tiling/mosaic.h>

#include <dak/geometry/face.h>
#include <dak/geometry/map.h>
#include <dak/geometry/rect.h>

#include <string>

namespace dak
{
   namespace tiling
   {
      using geometry::map;
      using geometry::rect;

      ////////////////////////////////////////////////////////////////////////////
      // 
      // Reads all tiling files in a given folder and generates them on-demand.

      class known_tilings_generator
      {
      public:
         known_tilings tilings;

         known_tilings_generator(const std::wstring& folder, std::vector<std::wstring>& errors);

         known_tilings_generator& next();

         known_tilings_generator& previous();

         known_tilings_generator& set_index(int index);

         const std::wstring current_name() const;

         const std::shared_ptr<mosaic>& current_mosaic() const { return mo; }

         map generate_tiling_map(const rect& region) const;

      private:
         void generate_mosaic();

         std::vector<tiling>::const_iterator iter;
         std::shared_ptr<mosaic> mo;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
