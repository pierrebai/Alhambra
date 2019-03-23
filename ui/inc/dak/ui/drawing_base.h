#pragma once

#ifndef DAK_UI_DRAWING_BASE_H
#define DAK_UI_DRAWING_BASE_H

#include <dak/ui/drawing.h>

namespace dak
{
   namespace ui
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // Base drawing implementation that manages color, stroke and transforms.

      class drawing_base : public ui::drawing
      {
      public:
         // drawing interface partially implemented.
         stroke get_stroke() const override;
         drawing_base& set_stroke(const stroke& s) override;

         color get_color() const override;
         drawing_base& set_color(const color& c) override;

         const transform& get_transform() const override;
         drawing_base& set_transform(const transform&) override;
         drawing_base& compose(const transform&) override;
         drawing_base& push_transform() override;
         drawing_base& pop_transform() override;

      private:
         color co = color::black();
         stroke strk = stroke(1);
         transform trf = transform::identity();
         std::vector<transform> saved_trfs;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
