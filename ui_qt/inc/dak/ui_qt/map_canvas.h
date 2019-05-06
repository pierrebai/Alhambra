#pragma once

#ifndef DAK_UI_QT_MAP_CANVAS_H
#define DAK_UI_QT_MAP_CANVAS_H

#include <dak/ui_qt/drawing_canvas.h>

#include <dak/geometry/map.h>

namespace dak
{
   namespace ui_qt
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // A widget drawing_canvas working with a map.

      class map_canvas : public drawing_canvas
      {
      public:
         // This is the layered container to draw on the drawing_canvas.
         geometry::map map;

         // Create a drawing_canvas with the given parent widget.
         map_canvas(QWidget* parent);

         // Draw the map in the given drawing.
         void draw_map(dak::ui::drawing& drw) const;

      protected:
         // This will draw the map, if any, then the transformer drawings, if any.
         void paint(QPainter& painter) override;

         void draw(drawing& drw) override;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
