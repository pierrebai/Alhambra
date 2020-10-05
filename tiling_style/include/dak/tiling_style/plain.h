#pragma once

#ifndef DAK_TILING_STYLE_PLAIN_H
#define DAK_TILING_STYLE_PLAIN_H

#include <dak/tiling_style/colored.h>

namespace dak
{
   namespace tiling_style
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // Render the map as a collection of colored line segments.

      class plain : public colored
      {
      public:
         // Creation.
         plain() { }
         plain(const ui::color& c) : colored(c) { }
         plain(const geometry::map& m, const ui::color& c) : colored(m, c) { }

         // Copy a layer.
         std::shared_ptr<layer> clone() const override;

         // Retrieve a description of this style.
         std::wstring describe() const override;

      protected:
         // The internal draw is called with the layer transform already applied.
         void internal_draw(ui::drawing& drw) override;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
