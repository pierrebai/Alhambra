#pragma once

#ifndef DAK_UI_MODIFIERS_H
#define DAK_UI_MODIFIERS_H

namespace dak
{
   namespace ui
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // Keyboard modifiers: shift, control and alt.

      enum class modifiers
      {
         none     = 0,
         shift    = 1,
         control  = 2,
         alt      = 4,
         all      = 7,
      };

      inline modifiers operator&(modifiers a, modifiers b) { return modifiers(int(a) & int(b)); }
      inline modifiers operator|(modifiers a, modifiers b) { return modifiers(int(a) | int(b)); }
      inline modifiers operator^(modifiers a, modifiers b) { return modifiers(int(a) ^ int(b)); }
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
