#pragma once

#ifndef DAK_UI_LAYERED_H
#define DAK_UI_LAYERED_H

#include <dak/ui/layer.h>

#include <vector>
#include <memory>

namespace dak
{
   namespace ui
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // Transformable container of multiple layers.
      // When transformed, transforms all its layers.

      class layered : public transformable
      {
      public:
         // Create a layers container.
         layered() { }

         // The layers.
         // The one at index zero is the highest when drawn (on top).
         typedef std::vector<std::shared_ptr<layer>> layers;
         const layers& get_layers() const { return _layers; }
         void set_layers(const layers& l);

         // Draw the layers.
         void draw(drawing& drw);

         // transformable implementation.
         const transform& get_transform() const override;
         layered& set_transform(const transform& t) override;
         layered& compose(const transform& t) override;

      private:
         transform trf = transform::identity();
         layers _layers;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
