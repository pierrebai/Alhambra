#pragma once

#ifndef DAK_TILING_STYLE_SKETCH_H
#define DAK_TILING_STYLE_SKETCH_H

#include <dak/tiling_style/colored.h>

namespace dak
{
   namespace tiling_style
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // Sketchy hand-drawn effect as a set of line segments whose
      // endpoints are jittered relative to the original edge.

      class sketch_t : public colored_t
      {
      public:
         // Creation.
         sketch_t() { }
         sketch_t(const ui::color& c) : colored_t(c) { }
         sketch_t(const geometry::map& m, const ui::color& c) : colored_t(m, c) { }

         // Copy a layer.
         std::shared_ptr<layer_t> clone() const override;

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
