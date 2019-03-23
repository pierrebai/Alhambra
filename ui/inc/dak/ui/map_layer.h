#pragma once

#ifndef DAK_UI_MAP_LAYER_H
#define DAK_UI_MAP_LAYER_H

#include <dak/ui/layer.h>

#include <dak/geometry/map.h>

namespace dak
{
   namespace ui
   {
      class drawing;

      ////////////////////////////////////////////////////////////////////////////
      //
      // A layer showing a map.

      class map_layer : public layer
      {
      public:
         geometry::map map;

         // Create a layer.
         map_layer() { }
         map_layer(geometry::map& map) :map(map) { }

         // Copy a layer.
         std::shared_ptr<layer> clone() const override;
         void make_similar(const layer& other) override;

      protected:
         // The internal draw is called with the layer transform already applied.
         void internal_draw(drawing& drw) override;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
