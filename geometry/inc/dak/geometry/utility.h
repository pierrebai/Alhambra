#pragma once

#ifndef DAK_GEOMETRY_UTILITY_H
#define DAK_GEOMETRY_UTILITY_H

#include <dak/geometry/number_utility.h>
#include <dak/geometry/point_utility.h>

#include <string>

namespace dak
{
   namespace geometry
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // Localisation.

      struct L
      {
         static void initialize();
         static const char* t(const char * text);
         static const wchar_t* t(const wchar_t * text);
      };

      std::wstring convert(const std::string& text);
      std::wstring convert(const char* text);
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 

