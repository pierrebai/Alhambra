#pragma once

#ifndef DAK_UTILITY_TEXT_H
#define DAK_UTILITY_TEXT_H

#include <string>

namespace dak
{
   namespace utility
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // Localisation.

      struct L
      {
         static void initialize();
         static void set_english();
         static void set_french();
         static const char* t(const char * text);
         static const wchar_t* t(const wchar_t * text);
      };

      std::wstring convert(const std::string& text);
      std::wstring convert(const char* text);
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 

