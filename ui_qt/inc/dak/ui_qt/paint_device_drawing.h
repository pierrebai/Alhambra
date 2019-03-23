#pragma once

#ifndef DAK_UI_QT_PAINT_DEVICE_DRAWING_H
#define DAK_UI_QT_PAINT_DEVICE_DRAWING_H

#include <dak/ui_qt/painter_drawing.h>

class QPaintDevice;

namespace dak
{
   namespace ui_qt
   {
      using geometry::point;
      using geometry::polygon;
      using geometry::rect;

      ////////////////////////////////////////////////////////////////////////////
      //
      // Draw on a QPaintDevice.

      class paint_device_drawing : public painter_drawing
      {
      public:
         // Create a drawing on the given QT paint device.
         paint_device_drawing(QPaintDevice& pd) : paint_device(pd) { }

         // ui::drawing interface, complete the implementation.
         paint_device_drawing& draw_line(const point& from, const point& to) override;
         paint_device_drawing& fill_polygon(const polygon& p) override;
         paint_device_drawing& draw_polygon(const polygon& p) override;
         paint_device_drawing& fill_oval(const point& c, double rx, double ry) override;
         paint_device_drawing& draw_oval(const point& c, double rx, double ry) override;
         paint_device_drawing& fill_arc(const point& c, double rx, double ry, double angle1, double angle2) override;
         paint_device_drawing& fill_rect(const rect& r) override;
         paint_device_drawing& draw_rect(const rect& r) override;

         rect get_bounds() const override;

      private:
         QPaintDevice& paint_device;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
