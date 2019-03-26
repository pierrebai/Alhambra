#pragma once

#ifndef DAK_UI_QT_CONVERT_H
#define DAK_UI_QT_CONVERT_H

#include <dak/geometry/point.h>
#include <dak/geometry/polygon.h>
#include <dak/geometry/rect.h>

#include <dak/ui/drawing.h>

#include <QtGui/qcolor.h>
#include <QtGui/qpolygon.h>

namespace dak
{
   namespace geometry
   {
      class point;
      class polygon;
   }

   namespace ui_qt
   {
      using ui::color;
      using ui::stroke;
      using geometry::point;
      using geometry::polygon;
      using geometry::rect;

      ////////////////////////////////////////////////////////////////////////////
      //
      // Convert various type to their QT equivalent.

      QColor convert(const color& c);
      QPointF convert(const point& p);
      QPolygonF convert(const polygon& p);
      Qt::PenCapStyle convert(stroke::cap_style c);
      Qt::PenJoinStyle convert(stroke::join_style j);
      QRectF convert(const rect & r);

      color convert(const QColor & c);
      point convert(const QPointF& p);
      polygon convert(const QPolygonF& p);
      rect convert(const QRect& r);
      rect convert(const QSize& s);
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
