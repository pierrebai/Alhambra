#pragma once

#ifndef DAK_TILING_INFER_MODE_H
#define DAK_TILING_INFER_MODE_H

#include <string>

namespace dak
{
   namespace tiling
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // Mode of inferring used to create figures.

      enum class infer_mode
      {
         star,
         girih,
         intersect,
         progressive,
         hourglass,
         rosette,
         extended_rosette,
         simple,
      };

      // Convert infer mode to and from a non-localized name.
      const wchar_t* infer_mode_name(infer_mode m);
      infer_mode infer_mode_from_name(const wchar_t* name);
      infer_mode infer_mode_from_name(const std::wstring& name);
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
