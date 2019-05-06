#pragma once

#ifndef DAK_UI_QT_MOSAIC_CANVAS_H
#define DAK_UI_QT_MOSAIC_CANVAS_H

#include <dak/ui_qt/drawing_canvas.h>

#include <dak/ui/layered.h>

namespace dak
{
   namespace ui_qt
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // A widget canvas working with a layers container.

      class layered_canvas : public drawing_canvas
      {
      public:
         // This is the layered container to draw on the canvas.
         ui::layered* layered = nullptr;

         // Create a canvas with the given parent widget.
         layered_canvas(QWidget* parent);

      protected:
         // This will draw the layered container, if any, then the transformer drawings, if any.
         void draw(drawing& drw) override;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
