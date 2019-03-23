#pragma once

#ifndef DAK_UI_QT_PAINT_DEVICE_TRANSFORMABLE_H
#define DAK_UI_QT_PAINT_DEVICE_TRANSFORMABLE_H

#include <dak/ui_qt/paint_device_drawing.h>

#include <dak/ui/transformable.h>

namespace dak
{
   namespace ui_qt
   {
      using geometry::point;
      using geometry::transform;

      ////////////////////////////////////////////////////////////////////////////
      //
      // Enable editing the transform of a QPaintDevice.

      class paint_device_transformable : public paint_device_drawing, public ui::transformable
      {
      public:
         // Create a transformable QT paint device.
         paint_device_transformable(QPaintDevice& pd) : paint_device_drawing(pd) { }

         // ui::transformable implementation.
         const transform& get_transform() const override;
         paint_device_transformable& set_transform(const transform& t) override;
         paint_device_transformable& compose(const transform& t) override;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
