#pragma once

#ifndef DAK_UI_QT_MOSAIC_CANVAS_H
#define DAK_UI_QT_MOSAIC_CANVAS_H

#include <dak/ui_qt/drawing_canvas.h>

#include <dak/tiling/mosaic.h>

#include <memory>

namespace dak
{
   namespace ui_qt
   {
      ////////////////////////////////////////////////////////////////////////////
      //
      // A widget drawing_canvas working with a mosaic.

      class mosaic_canvas : public drawing_canvas
      {
      public:
         // This is the mosaic to draw on the drawing_canvas.
         std::shared_ptr<tiling::mosaic> mosaic;

         // Create a drawing_canvas with the given parent widget.
         mosaic_canvas(QWidget* parent);

      protected:
         // Paint the mosaic using the prepared drawing.
         void draw(drawing& drw) override;
      };
   }
}

#endif

// vim: sw=3 : sts=3 : et : sta : 
