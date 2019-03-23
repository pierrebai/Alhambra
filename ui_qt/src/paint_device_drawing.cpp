#include <dak/ui_qt/paint_device_drawing.h>
#include <dak/ui_qt/convert.h>

#include <QtGui/qpainter.h>
#include <QtGui/qpainterpath.h>

namespace dak
{
   namespace ui_qt
   {
      paint_device_drawing& paint_device_drawing::draw_line(const point& from, const point& to)
      {
         QPainter tmp_painter(&paint_device);
         painter = &tmp_painter;
         painter_drawing::draw_line(from, to);
         painter = 0;
         return *this;
      }

      paint_device_drawing& paint_device_drawing::fill_polygon(const polygon& p)
      {
         QPainter tmp_painter(&paint_device);
         painter = &tmp_painter;
         painter_drawing::fill_polygon(p);
         painter = 0;
         return *this;
      }

      paint_device_drawing& paint_device_drawing::draw_polygon(const polygon& p)
      {
         QPainter tmp_painter(&paint_device);
         painter = &tmp_painter;
         painter_drawing::draw_polygon(p);
         painter = 0;
         return *this;
      }

      paint_device_drawing& paint_device_drawing::fill_oval(const point& c, double rx, double ry)
      {
         QPainter tmp_painter(&paint_device);
         painter = &tmp_painter;
         painter_drawing::fill_oval(c, rx, ry);
         painter = 0;
         return *this;
      }

      paint_device_drawing& paint_device_drawing::draw_oval(const point& c, double rx, double ry)
      {
         QPainter tmp_painter(&paint_device);
         painter = &tmp_painter;
         painter_drawing::draw_oval(c, rx, ry);
         painter = 0;
         return *this;
      }

      paint_device_drawing& paint_device_drawing::fill_arc(const point& c, double rx, double ry, double angle1, double angle2)
      {
         QPainter tmp_painter(&paint_device);
         painter = &tmp_painter;
         painter_drawing::fill_arc(c, rx, ry, angle1, angle2);
         painter = 0;
         return *this;
      }

      paint_device_drawing& paint_device_drawing::fill_rect(const rect& r)
      {
         QPainter tmp_painter(&paint_device);
         painter = &tmp_painter;
         painter_drawing::fill_rect(r);
         painter = 0;
         return *this;
      }

      paint_device_drawing& paint_device_drawing::draw_rect(const rect& r)
      {
         QPainter tmp_painter(&paint_device);
         painter = &tmp_painter;
         painter_drawing::draw_rect(r);
         painter = 0;
         return *this;
      }

      rect paint_device_drawing::get_bounds() const
      {
         return rect(0, 0, paint_device.width(), paint_device.height());
      }
   }
}

// vim: sw=3 : sts=3 : et : sta : 
