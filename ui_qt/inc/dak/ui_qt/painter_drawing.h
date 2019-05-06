#pragma once

#ifndef DAK_UI_QT_PAINTER_DRAWING_H
#define DAK_UI_QT_PAINTER_DRAWING_H

#include <dak/ui_qt/qt_drawing.h>

class QPainter;

namespace dak
{
   namespace ui_qt
   {
      using geometry::point;
      using geometry::polygon;
      using geometry::rect;

      ////////////////////////////////////////////////////////////////////////////
      //
      // Draw on a QPainter.

      class painter_drawing : public qt_drawing
      {
      public:
         // Current QPainter. Nothing drawn if null.
         QPainter* painter;

         // Empty drawing.
         painter_drawing() : painter(nullptr) { }

         // Drawing on the given QPainter.
         painter_drawing(QPainter& p) : painter(&p) { }

         // ui::drawing interface, complete the implementation.
         painter_drawing& draw_line(const point& from, const point& to) override;
         painter_drawing& draw_corner(const point& from, const point& corner, const point& to) override;
         painter_drawing& fill_polygon(const polygon& p) override;
         painter_drawing& draw_polygon(const polygon& p) override;
         painter_drawing& fill_oval(const point& c, double rx, double ry) override;
         painter_drawing& draw_oval(const point& c, double rx, double ry) override;
         painter_drawing& fill_arc(const point& c, double rx, double ry, double angle1, double angle2) override;
         painter_drawing& fill_rect(const rect& r) override;
         painter_drawing& draw_rect(const rect& r) override;

         rect get_bounds() const override;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
