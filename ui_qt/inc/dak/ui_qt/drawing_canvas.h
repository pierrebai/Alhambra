#pragma once

#ifndef DAK_UI_QT_DRAWING_CANVAS_H
#define DAK_UI_QT_DRAWING_CANVAS_H

#include <dak/ui_qt/canvas.h>

#include <dak/ui_qt/painter_transformable.h>

namespace dak
{
   namespace ui_qt
   {
      using ui::drawing;

      ////////////////////////////////////////////////////////////////////////////
      //
      // A widget canvas working with a transformable painter drawing.

      class drawing_canvas : public canvas
      {
      public:
         // Create a canvas with the given parent widget.
         drawing_canvas(QWidget* parent);
         drawing_canvas(QWidget* parent, ui::transformable& trfable);

      protected:
         // This will call draw, then the transformer drawings, if any.
         void paint(QPainter& painter) override;

         // Paint using the prepared drawing.
         // Sub-class should override this to draw in the canvas.
         virtual void draw(drawing& drw);

         // This will allow painting using a drawing.
         dak::ui_qt::painter_transformable painter_trf_drawing;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
