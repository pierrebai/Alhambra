#pragma once

#ifndef DAK_UI_QT_TILING_CANVAS_H
#define DAK_UI_QT_TILING_CANVAS_H

#include <dak/ui_qt/drawing_canvas.h>

#include <dak/tiling/tiling.h>

namespace dak
{
   namespace ui_qt
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // A widget drawing_canvas working with a tiling.

      class tiling_canvas : public drawing_canvas
      {
      public:
         // This is the tiling to draw on the drawing_canvas.
         tiling::tiling tiling;

         // Create a drawing_canvas with the given parent widget.
         tiling_canvas(QWidget* parent);

      protected:
         // Paint using the prepared drawing.
         void draw(drawing& drw) override;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
