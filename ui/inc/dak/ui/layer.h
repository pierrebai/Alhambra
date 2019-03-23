#pragma once

#ifndef DAK_UI_LAYER_H
#define DAK_UI_LAYER_H

#include <dak/ui/transformable.h>

#include <memory>

namespace dak
{
   namespace ui
   {
      class drawing;

      ////////////////////////////////////////////////////////////////////////////
      //
      // A drawable, transformable layer.
      //
      // Sub-class must provide the internal draw implementation.

      class layer : public transformable
      {
      public:
         // Create a layer.
         layer() { }

         // Copy a layer.
         virtual std::shared_ptr<layer> clone() const = 0;
         virtual void make_similar(const layer& other) = 0;

         // Draw the transformed layer.
         void draw(drawing& drw);

         // transformable implementation.
         const transform& get_transform() const override { return trf; }
         layer& set_transform(const transform& t) override { trf = t; return *this; }
         layer& compose(const transform& t) override { trf = trf.compose(t); return *this; }

      protected:
         // The internal draw is called with the layer transform already applied.
         virtual void internal_draw(drawing& drw) = 0;

      private:
         transform trf = transform::identity();
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
